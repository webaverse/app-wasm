#include "AnimationSystem.h"

namespace AnimationSystem
{
  // std::map<std::string, Interpolant *> interpolants;
  // std::vector<Interpolant> _interpolants;
  std::vector<AnimationMixer> _animationMixers;
  std::vector<AnimationMapping> _animationMappings;
  std::vector<Animation *> _animations;
  // std::vector<AnimationNode *> _motions;
  float *_animationValues[55]; // 53 bones interpolants result buffers + 1 finished event flag + 1 finished animation index.
  float finishedFlag = 0;
  float finishedAnimationIndex; // todo: use pointer instead of index.
  // float **_animationValues = (float **)malloc(53 * sizeof(float)); // ok too
  // Interpolant _interpolant;
  AnimationNode *_animTree;
  float *test;
  float **test2;
  unsigned int **test3;
  Animation **test4;

  // functions:

  float setTest(float num)
  {
    float a = num;
    test = &a;
    return *test;
  }

  float setTest2(float num)
  {
    *test = num;
    return *test;
  }

  float setTestNew(float num)
  {
    test = new float(num);
    return *test;
  }

  float setTestAlloc(float num)
  {
    test = (float *)malloc(1 * sizeof(float));
    *test = num;
    return *test;
  }

  float getTest()
  {
    return *test;
  }

  // Utils ------

  float min(float a, float b)
  {
    if (a > b)
    {
      return b;
    }
    else
    {
      return a;
    }
  }
  float max(float a, float b)
  {
    if (a > b)
    {
      return a;
    }
    else
    {
      return b;
    }
  }

  // Main ------

  // float changeWeight(unsigned int animationIndex, float weight)
  // {
  //   // // no effect if _animations not store pointer.
  //   // Animation animation = _animations[animationIndex];
  //   // Animation animation = _animTree.children[animationIndex];
  //   // animation.weight = weight;

  //   _animTree->children[animationIndex]->weight = weight; // todo: test: animationIndex is 0 | 1 here, not real animationIndex.
  //   return weight;
  // }

  void changeWeight(AnimationNode *node, float weight)
  {
    node->weight = weight;
  }

  void changeFactor(AnimationNode *node, float factor)
  {
    node->factor = factor;
  }

  float getWeight(AnimationSystem::AnimationNode *node)
  {
    return node->weight;
  }

  float getFactor(AnimationSystem::AnimationNode *node)
  {
    return node->factor;
  }

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

    // // init old
    // AnimationNode *walkFlyNode = new AnimationNode();
    // walkFlyNode->children.push_back(_motions[96]); // 96 walk
    // walkFlyNode->children.push_back(_motions[92]); // 92 fly

    // AnimationNode *crouchNode = new AnimationNode();
    // crouchNode->children.push_back(walkFlyNode);
    // crouchNode->children.push_back(_motions[9]); // 9 Crouch Idle.fbx

    // _animTree = crouchNode;

    // // init
    // AnimationNode *walkMotion = createMotion(_animations[96]);
    // AnimationNode *flyMotion = createMotion(_animations[92]);
    // AnimationNode *crouchMotion = createMotion(_animations[9]);

    // AnimationNode *walkFlyNode = createNode();
    // addChild(walkFlyNode, walkMotion);
    // addChild(walkFlyNode, flyMotion);

    // AnimationNode *crouchNode = createNode();
    // addChild(crouchNode, walkFlyNode);
    // addChild(crouchNode, crouchMotion);

    // setAnimTree(crouchNode);
  }
  AnimationNode *createNode(NodeType type)
  {
    AnimationNode *node = new AnimationNode();
    node->type = type;
    std::cout << "NodeType: " << type << " " << node->type << std::endl;
    return node;
  }
  void addChild(AnimationNode *parent, AnimationNode *child)
  {
    parent->children.push_back(child);

    if (parent->type == NodeType::UNITARY)
    {
      if (parent->children.size() == 1)
      {
        parent->activeNode = child;
        child->weight = 1;
      }
      else
      {
        child->weight = 0;
      }
    }
  }
  void setAnimTree(AnimationNode *node)
  {
    _animTree = node;
  }
  float **updateAnimationMixer(float timeS, float f)
  {
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
    // std::cout << "_animationMappings size: " << _animationMappings.size() << std::endl;
  }
  Animation *createAnimation(float duration)
  {
    Animation *animation = new Animation();
    animation->index = _animations.size();
    animation->duration = duration;
    _animations.push_back(animation);
    // std::cout << "_animations size: " << _animations.size() << std::endl;

    return animation;
  }
  Animation *getAnimation1(unsigned int index)
  {
    return _animations[index];
  }
  float *getAnimation2(unsigned int index)
  {
    return (float *)(_animations[index]);
  }
  unsigned int *getAnimation3(unsigned int index)
  {
    return (unsigned int *)(_animations[index]);
  }
  float **getAnimation4(unsigned int index)
  {
    *test2 = (float *)(_animations[index]);
    return test2;
  }
  unsigned int **getAnimation5(unsigned int index)
  {
    *test3 = (unsigned int *)(_animations[index]);
    return test3;
  }
  Animation **getAnimation6(unsigned int index)
  {
    *test4 = _animations[index];
    return test4;
  }
  AnimationNode *createMotion(Animation *animation)
  {
    AnimationNode *motion = new AnimationNode();
    motion->animation = animation;
    // _motions.push_back(motion);

    return motion;
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
    _animations[animationIndex]->interpolants.push_back(interpolant);

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

    Interpolant interpolant = _animations[animationIndex]->interpolants[interpolantIndex];

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
  // void crossFade(AnimationNode *parentNode, float duration, AnimationNode *targetNode)
  // {
  //   parentNode->crossFade(duration, targetNode);
  // }
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
    AnimationMixer::timeS = timeS;

    // reset
    finishedFlag = 0; // reset animation finished event.

    // return getAnimationValues(_animation.index, timeS); // Move `getAnimationValues()` to class AnimationMixer.

    for (int i = 0; i < 53; i++)
    {
      AnimationMapping spec = _animationMappings[i];

      // float * aaa = _animTree.update(spec);
      _animationValues[i] = _animTree->update(spec);
    }

    _animationValues[53] = &finishedFlag;
    _animationValues[54] = &finishedAnimationIndex;

    return _animationValues;
  }

  float *AnimationNode::update(AnimationMapping &spec) // todo: &spec
  {
    if (this->animation) // isMotion ------
    {
      float *value;
      float evaluateTimeS;
      // std::cout << "loop: " << this->loop << std::endl;
      if (this->loop == LoopType::LoopOnce)
      {
        evaluateTimeS = (AnimationMixer::timeS - this->startTime) / this->speed + this->timeBias;
        value = evaluateInterpolant(this->animation->index, spec.index, evaluateTimeS);
        // std::cout << "evaluateTimeS: " << evaluateTimeS << std::endl;
        // if (isLastBone && this->weight > 0 && !this->isFinished && evaluateTimeS >= this->animation.duration)
        // {
        //   // console.log('finished', this->name);
        //   this->mixer.dispatchEvent({
        //     type : 'finished',
        //     motion : this,
        //   });

        //   this->isFinished = true;
        // }
        if (spec.isLastBone && !this->isFinished && evaluateTimeS >= this->animation->duration)
        {
          std::cout << "finished: index: " << this->animation->index << " pointer: " << this->animation << std::endl;
          // value[0] *= -1; // todo: Use bit mask instead of negative.
          finishedFlag = 1;
          finishedAnimationIndex = (float)this->animation->index; // must explicitly convert index (unsigned int) to float, otherwise will cause wrong value.

          this->isFinished = true;
        }
      }
      else
      {
        evaluateTimeS = fmod((AnimationMixer::timeS - this->startTime) / this->speed + this->timeBias, this->animation->duration);
        value = evaluateInterpolant(this->animation->index, spec.index, evaluateTimeS);
      }
      return value;
    }
    else // isNode ------
    {

      if (this->type == NodeType::TWO)
      {
        if (this->isCrossFade)
        {
          this->factor = (AnimationMixer::timeS - this->crossFadeStartTime) / this->crossFadeDuration;
          this->factor = min(max(this->factor, 0), 1);
          if (this->crossFadeTargetFactor == 0)
          {
            this->factor = 1 - this->factor;
          }
          if (this->factor == this->crossFadeTargetFactor)
            this->isCrossFade = false;
        }
        this->children[0]->weight = 1 - this->factor;
        this->children[1]->weight = this->factor;
      }
      else if (this->type == NodeType::UNITARY)
      {
        if (this->isCrossFade)
        {
          float factor = (AnimationMixer::timeS - this->crossFadeStartTime) / this->crossFadeDuration;
          factor = min(max(factor, 0), 1);
          float factorReverse = 1 - factor;

          for (int i = 0; i < this->children.size(); i++)
          {
            AnimationNode *childNode = this->children[i];
            if (childNode == this->activeNode)
            {
              // childNode->weight = factor;
              childNode->weight = max(childNode->weight, factor);
            }
            else
            {                                                            // ensure unitary
              childNode->weight = min(childNode->weight, factorReverse); // todo: will cause jumpping values if last crossFade() hasn't finished.
              // childNode->weight = childNode->weightStart * factorReverse;
            }
          }
          // if (spec.isFirstBone)
          // {
          //   std::cout << "factor: " << factor << std::endl;
          // }

          if (factor == 1)
          {
            this->isCrossFade = false;
          }
        }
      }

      // doBlendList ---
      float *result;
      unsigned int nodeIndex = 0;
      float currentWeight = 0;
      for (int i = 0; i < this->children.size(); i++)
      {
        AnimationNode *childNode = this->children[i]; // todo: If not using pointer, cpp will copy node data when assign here? Yes.
        if (childNode->weight > 0)
        {
          float *value = childNode->update(spec);
          if (nodeIndex == 0)
          {
            result = value;

            nodeIndex++;
            currentWeight = childNode->weight;
          }
          else
          {
            float t = childNode->weight / (currentWeight + childNode->weight);
            interpolateFlat(result, 1, result, 1, value, 1, t, spec.isPosition);

            nodeIndex++;
            currentWeight += childNode->weight;
          }
        }
      }
      return result;
    }
  }
  void AnimationNode::crossFadeTwo(float duration, float factor)
  {
    this->isCrossFade = true;
    this->crossFadeStartTime = AnimationMixer::timeS;
    this->crossFadeDuration = duration;
    this->crossFadeTargetFactor = factor;
  }
  void AnimationNode::crossFadeUnitary(float duration, AnimationNode *targetNode)
  {
    this->isCrossFade = true;
    this->crossFadeStartTime = AnimationMixer::timeS;
    this->crossFadeDuration = duration;
    this->activeNode = targetNode;
  }
  void AnimationNode::play()
  {
    this->weight = abs(this->weight);
    this->startTime = AnimationMixer::timeS;
    this->isFinished = false;
  }
  void AnimationNode::stop()
  {
    this->weight = -abs(this->weight);
  }
  void AnimationNode::setTimeBias(float timeBias)
  {
    this->timeBias = timeBias;
  }
  void AnimationNode::setSpeed(float speed)
  {
    this->speed = speed;
  }
  void AnimationNode::setLoop(LoopType loopType)
  {
    this->loop = loopType;
  }
}