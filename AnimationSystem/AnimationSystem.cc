#include "AnimationSystem.h"

namespace AnimationSystem
{
  // std::map<std::string, Interpolant *> interpolants;
  // std::vector<Interpolant> _interpolants;
  std::vector<AnimationMixer> _animationMixers;
  std::vector<AnimationMapping> _animationMappings;
  Animation _animation;
  std::vector<Animation> _animations;
  std::vector<AnimationMotion> _motions;
  float *_animationValues[53];
  // float **_animationValues = (float **)malloc(53 * sizeof(float)); // ok too
  // Interpolant _interpolant;
  AnimationNode animTree;

  // functions:

  void interpolateFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t, bool isPosition)
  {
    if (isPosition)
    {
      lerpFlat(dst, dstOffset, src0, srcOffset0, src1, srcOffset1, t);
    }
    else
    {
      slerpFlat(dst, dstOffset, src0, srcOffset0, src1, srcOffset1, t);
    }
  }

  // AnimationMixer *createAnimationMixer(unsigned int avatarId) {
  void createAnimationMixer(unsigned int avatarId)
  {
    AnimationMixer animationMixer(avatarId);
    _animationMixers.push_back(animationMixer);
    // return &animationMixer; // todo: warning: address of stack memory associated with local variable 'animationMixer' returned [-Wreturn-stack-address]

    // init
    AnimationNode node;
    node.index = 7;
    node.children.push_back(_motions[92]);
    node.children.push_back(_motions[96]);
    std::cout << "node.children.size(): " << node.children.size() << std::endl;
    std::cout << "node.index: " << node.index << std::endl;
    // animationMixer.animTree = node;
    animTree = node;
    // std::cout << "animTree pointer 1: " << animationMixer.animTree << std::endl;
  }
  float **updateAnimationMixer(float timeS, float f)
  {
    std::cout << "updateAnimationMixer" << std::endl;
    return _animationMixers[0].update(timeS, f);
  }
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone)
  {
    AnimationMapping animationMapping;
    animationMapping.isPosition = isPosition;
    animationMapping.index = index;
    animationMapping.isFirstBone = isFirstBone;
    animationMapping.isLastBone = isLastBone;
    _animationMappings.push_back(animationMapping);
    std::cout << "_animationMappings size: " << _animationMappings.size() << std::endl;
  }
  void createAnimation(float duration)
  {
    Animation animation;
    animation.index = _animations.size();
    animation.duration = duration;
    _animations.push_back(animation);
    std::cout << "_animations size: " << _animations.size() << std::endl;

    // todo: move to seperate function
    AnimationMotion motion;
    motion.animation = animation;
    _motions.push_back(motion);
  }
  void createInterpolant(unsigned int animationIndex, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize)
  {

    // std::cout << "createInterpolant: " << numParameterPositions << " " << numSampleValues << " " << valueSize << std::endl;

    Interpolant interpolant;
    interpolant.numParameterPositions = numParameterPositions;
    interpolant.parameterPositions = parameterPositions;
    interpolant.resultBuffer = (float *)malloc((1 + valueSize) * sizeof(float)); // 1 len for valueSize self ( 3 or 4 ). 3 len or 4 len for values.
    interpolant.numSampleValues = numSampleValues;
    interpolant.sampleValues = sampleValues;
    interpolant.valueSize = valueSize; // only support 3 (vector) or 4 (quaternion)

    // _interpolant = interpolant;
    _animations[animationIndex].interpolants.push_back(interpolant);

    // std::cout
    // << "interpolant "
    // << interpolant.numParameterPositions
    // << interpolant.numSampleValues
    // << interpolant.valueSize
    // << _interpolants.size()
    // << std::endl;

    // std::cout << "interpolants size: " << _interpolants.size() << std::endl;
  }
  float *evaluateInterpolant(unsigned int animationIndex, unsigned int interpolantIndex, float t)
  {
    // std::cout << "evaluateInterpolant: " << interpolantIndex << " " << t << std::endl;

    // return _sampleValues[(int)t] + _parameterPositions[(int)t] + _valueSize;

    Interpolant interpolant = _animations[animationIndex].interpolants[interpolantIndex];

    if (interpolant.numParameterPositions == 1)
    {
      interpolant.resultBuffer[0] = interpolant.valueSize;
      interpolant.resultBuffer[1] = interpolant.sampleValues[0];
      interpolant.resultBuffer[2] = interpolant.sampleValues[1];
      interpolant.resultBuffer[3] = interpolant.sampleValues[2];
      if (interpolant.valueSize == 4)
      {
        interpolant.resultBuffer[4] = interpolant.sampleValues[3];
      }
    }
    else
    {
      int index = 0;
      // std::cout << "numParameterPositions: " << interpolant.numParameterPositions << std::endl;
      for (; index < interpolant.numParameterPositions; index++)
      {
        // std::cout << "index: " << index << " position: " << interpolant.parameterPositions[index] << std::endl;
        if (interpolant.parameterPositions[index] > t)
        {
          break;
        }
      }
      // index -= 1; // evaluate floor
      // if (interpolantIndex == 1) std::cout << "index: " << index << std::endl;
      // std::cout << "index: " << index << std::endl;

      if (index == 0)
      { // Handle situation that, parameterPositions[0] > 0, and t == 0 or t < parameterPositions[0].
        interpolant.resultBuffer[0] = interpolant.valueSize;
        interpolant.resultBuffer[1] = interpolant.sampleValues[0];
        interpolant.resultBuffer[2] = interpolant.sampleValues[1];
        interpolant.resultBuffer[3] = interpolant.sampleValues[2];
        if (interpolant.valueSize == 4)
        {
          interpolant.resultBuffer[4] = interpolant.sampleValues[3];
        }
      }
      else if (index > interpolant.numParameterPositions - 1)
      { // Handle situation that, t > max parameterPosition.
        unsigned int maxIndex = interpolant.numParameterPositions - 1;
        interpolant.resultBuffer[0] = interpolant.valueSize;
        interpolant.resultBuffer[1] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 0];
        interpolant.resultBuffer[2] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 1];
        interpolant.resultBuffer[3] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 2];
        if (interpolant.valueSize == 4)
        {
          interpolant.resultBuffer[4] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 3];
        }
      }
      else
      {
        unsigned int index0 = index - 1;
        unsigned int index1 = index;

        // if (interpolantIndex == 33) { // mixamorigRightHandThumb1.quaternion
        //   std::cout << "index: " << index << std::endl; // always 1
        // }

        // float *outputBuffer = (float *)malloc((
        //   4
        // ) * sizeof(float));

        // outputBuffer[0] = _parameterPositions[index];
        // outputBuffer[1] = _sampleValues[index];
        // outputBuffer[2] = _valueSize;

        // float x0 = interpolant.sampleValues[index0 * interpolant.valueSize + 0];
        // float y0 = interpolant.sampleValues[index0 * interpolant.valueSize + 1];
        // float z0 = interpolant.sampleValues[index0 * interpolant.valueSize + 2];
        // float w0 = interpolant.sampleValues[index0 * interpolant.valueSize + 3];

        // float x1 = interpolant.sampleValues[index1 * interpolant.valueSize + 0];
        // float y1 = interpolant.sampleValues[index1 * interpolant.valueSize + 1];
        // float z1 = interpolant.sampleValues[index1 * interpolant.valueSize + 2];
        // float w1 = interpolant.sampleValues[index1 * interpolant.valueSize + 3];

        float time0 = interpolant.parameterPositions[index0];
        float time1 = interpolant.parameterPositions[index1];
        float f = (t - time0) / (time1 - time0);

        if (interpolant.valueSize == 3)
        {
          interpolant.resultBuffer[0] = 3;
          lerpFlat(
              interpolant.resultBuffer, 1,
              interpolant.sampleValues, index0 * interpolant.valueSize,
              interpolant.sampleValues, index1 * interpolant.valueSize,
              f);
        }
        else
        {
          interpolant.resultBuffer[0] = 4;
          slerpFlat(
              interpolant.resultBuffer, 1,
              interpolant.sampleValues, index0 * interpolant.valueSize,
              interpolant.sampleValues, index1 * interpolant.valueSize,
              f);
        }

        // interpolant.resultBuffer[0] = interpolant.sampleValues[index * interpolant.valueSize + 0];
        // interpolant.resultBuffer[1] = interpolant.sampleValues[index * interpolant.valueSize + 1];
        // interpolant.resultBuffer[2] = interpolant.sampleValues[index * interpolant.valueSize + 2];
        // interpolant.resultBuffer[3] = interpolant.sampleValues[index * interpolant.valueSize + 3];

        // outputBuffer[3] = (float)index;
      }
    }

    return interpolant.resultBuffer;
  }
  float **getAnimationValues(unsigned int animationIndex, float t)
  {
    for (int i = 0; i < 53; i++)
    {
      AnimationMapping spec = _animationMappings[i];
      _animationValues[i] = evaluateInterpolant(animationIndex, i, t);
    }
    return _animationValues;
  }
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t)
  {
    float x0 = src0[srcOffset0 + 0];
    float y0 = src0[srcOffset0 + 1];
    float z0 = src0[srcOffset0 + 2];

    float x1 = src1[srcOffset1 + 0];
    float y1 = src1[srcOffset1 + 1];
    float z1 = src1[srcOffset1 + 2];

    dst[dstOffset + 0] = x0 + (x1 - x0) * t;
    dst[dstOffset + 1] = y0 + (y1 - y0) * t;
    dst[dstOffset + 2] = z0 + (z1 - z0) * t;
  };
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t)
  {

    // fuzz-free, array-based Quaternion SLERP operation

    float x0 = src0[srcOffset0 + 0],
          y0 = src0[srcOffset0 + 1],
          z0 = src0[srcOffset0 + 2],
          w0 = src0[srcOffset0 + 3];

    float x1 = src1[srcOffset1 + 0],
          y1 = src1[srcOffset1 + 1],
          z1 = src1[srcOffset1 + 2],
          w1 = src1[srcOffset1 + 3];

    if (t == 0)
    {

      dst[dstOffset + 0] = x0;
      dst[dstOffset + 1] = y0;
      dst[dstOffset + 2] = z0;
      dst[dstOffset + 3] = w0;
      return;
    }

    if (t == 1)
    {

      dst[dstOffset + 0] = x1;
      dst[dstOffset + 1] = y1;
      dst[dstOffset + 2] = z1;
      dst[dstOffset + 3] = w1;
      return;
    }

    if (w0 != w1 || x0 != x1 || y0 != y1 || z0 != z1)
    {

      float s = 1 - t;
      float cos = x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1,
            dir = (cos >= 0 ? 1 : -1),
            sqrSin = 1 - cos * cos;

      // Skip the Slerp for tiny steps to avoid numeric problems:
      float EPSILON = 2.220446049250313e-16;
      if (sqrSin > EPSILON)
      {

        float sinVal = sqrt(sqrSin),
              len = atan2(sinVal, cos * dir);

        s = sin(s * len) / sinVal;
        t = sin(t * len) / sinVal;
      }

      float tDir = t * dir;

      x0 = x0 * s + x1 * tDir;
      y0 = y0 * s + y1 * tDir;
      z0 = z0 * s + z1 * tDir;
      w0 = w0 * s + w1 * tDir;

      // Normalize in case we just did a lerp:
      if (s == 1 - t)
      {

        float f = 1 / sqrt(x0 * x0 + y0 * y0 + z0 * z0 + w0 * w0);

        x0 *= f;
        y0 *= f;
        z0 *= f;
        w0 *= f;
      }
    }

    dst[dstOffset] = x0;
    dst[dstOffset + 1] = y0;
    dst[dstOffset + 2] = z0;
    dst[dstOffset + 3] = w0;
  }
  float **AnimationMixer::update(float timeS, float f /*test*/)
  {
    std::cout << "AnimationMixer::update" << std::endl;
    AnimationMixer::timeS = timeS;

    // return getAnimationValues(_animation.index, timeS); // Move `getAnimationValues()` to class AnimationMixer.

    for (int i = 0; i < 53; i++)
    {
      AnimationMapping spec = _animationMappings[i];
      // float t0 = fmod(timeS, _animations[92].duration);
      // float t1 = fmod(timeS, _animations[96].duration);
      // float *v0 = evaluateInterpolant(92, i, t0); // 92 fly
      // float *v1 = evaluateInterpolant(96, i, t1); // 96 walk
      // // if (i == 1) std::cout << timeS << " " << _animations[92].duration << " " << t0 << " " << t1 << " " << v0[0] << " " << v1[0] << std::endl;
      // // _animationValues[i] = evaluateInterpolant(_animation.index, i, timeS);
      // if (spec.isPosition)
      // {
      //   lerpFlat(v0, 1, v0, 1, v1, 1, f);
      // }
      // else
      // {
      //   slerpFlat(v0, 1, v0, 1, v1, 1, f);
      // }
      // _animationValues[i] = v0;

      // float *result = doBlend(this->animTree, spec);
      // std::cout << "animTree pointer 2: " << this->animTree << std::endl;
      // float *result = this->animTree.update(spec); // todo: referrence transfer ( not pointer );
      std::cout << "size: " << animTree.children.size() << std::endl;
      std::cout << "node.index: " << animTree.index << std::endl;
      float *result = animTree.update(spec);
    }
    return _animationValues;
  }
  // float *AnimationMixer::doBlend(AnimationNode *node, AnimationMapping *spec)
  // {
  //   AnimationNode *node = node;
  //   float *value = node->update(spec);
  //   return value;
  // }

  float *AnimationMotion::update(AnimationMapping spec)
  {
    std::cout << "AnimationMotion::update" << std::endl;
    float time;
    if (spec.isFirstBone)
    {
      time = AnimationMixer::timeS - this->startTime;
    }

    float *value;
    float evaluateTimeS = std::fmod(AnimationMixer::timeS / this->speed + this->timeBias, this->animation.duration);
    value = evaluateInterpolant(this->animation.index, spec.index, evaluateTimeS);

    return value;
  }

  float *AnimationNode::update(AnimationMapping spec)
  {
    std::cout << "AnimationNode::update" << std::endl;
    float *result = doBlendList(spec);
    return result;
  }

  float *AnimationNode::doBlendList(AnimationMapping spec)
  {
    std::cout << "AnimationNode::doBlendList" << std::endl;
    float *result;
    unsigned int nodeIndex = 0;
    float currentWeight = 0;
    std::cout << (this == &animTree) << std::endl;
    std::cout << "node.children.size(): " << this->children.size() << std::endl;
    std::cout << "node.index: " << this->index << std::endl;
    for (int i = 0; i < this->children.size(); i++)
    {
      std::cout << "i: " << i << std::endl;
      AnimationMotion childNode = this->children[i];
      if (childNode.weight > 0)
      {
        // float *value = this->mixer.doBlend(childNode, spec);
        float *value = childNode.update(spec);

        if (nodeIndex == 0)
        {
          result = value;

          nodeIndex++;
          currentWeight = childNode.weight;
        }
        else
        {
          float t = childNode.weight / (currentWeight + childNode.weight);
          interpolateFlat(result, 1, result, 1, value, 1, t, spec.isPosition);

          nodeIndex++;
          currentWeight += childNode.weight;
        }
      }
    }
    return result;
  }
}