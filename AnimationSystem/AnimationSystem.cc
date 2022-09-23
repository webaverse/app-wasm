#include "AnimationSystem.h"

namespace AnimationSystem {
  std::vector<Avatar *> avatars;
  std::vector<AnimationMixer *> _animationMixers;
  std::vector<AnimationMapping> _animationMappings;
  std::map<std::string, Animation *> animationAll;
  std::map<std::string, std::map<std::string, Animation *>> animationGroups; // todo: performance: all use unsigned int as key ?
  std::map<unsigned int, std::string> AnimationName;
  std::map<std::string, float> speedFactors;

  std::string defaultSitAnimationName;
  std::string defaultEmoteAnimationName;
  std::string defaultDanceAnimationName;
  std::string defaultHoldAnimationName;
  std::string defaultActivateAnimationName;
  std::string defaultNarutoRunAnimationName;

  float localVectorArr[3];
  float localQuaternionArr[4];
  float localVecQuatArr[4];

  float *localVecQuatPtr;
  float *localVecQuatPtr2;

  std::map<std::string, float> localWeights;

  float identityQuaternion[4] = {0, 0, 0, 1};

  // functions:

  // Utils ------

  float min(float a, float b) {
    if (a > b) {
      return b;
    } else {
      return a;
    }
  }
  float max(float a, float b) {
    if (a > b) {
      return a;
    } else {
      return b;
    }
  }
  float clamp(float value, float minValue, float maxValue) {
    return max( minValue, min( maxValue, value ) );
  }
  void _clearXZ(float *dst, bool isPosition) {
    if (isPosition) {
      dst[0] = 0;
      dst[2] = 0;
    }
  }
  void copyValue(float *dst, float *src, bool isPosition) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    if (!isPosition) dst[3] = src[3];
  }
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t) {
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
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t) {

    // fuzz-free, array-based Quaternion SLERP operation

    float x0 = src0[srcOffset0 + 0],
          y0 = src0[srcOffset0 + 1],
          z0 = src0[srcOffset0 + 2],
          w0 = src0[srcOffset0 + 3];

    float x1 = src1[srcOffset1 + 0],
          y1 = src1[srcOffset1 + 1],
          z1 = src1[srcOffset1 + 2],
          w1 = src1[srcOffset1 + 3];

    if (t == 0) {

      dst[dstOffset + 0] = x0;
      dst[dstOffset + 1] = y0;
      dst[dstOffset + 2] = z0;
      dst[dstOffset + 3] = w0;
      return;
    }

    if (t == 1) {

      dst[dstOffset + 0] = x1;
      dst[dstOffset + 1] = y1;
      dst[dstOffset + 2] = z1;
      dst[dstOffset + 3] = w1;
      return;
    }

    if (w0 != w1 || x0 != x1 || y0 != y1 || z0 != z1) {

      float s = 1 - t;
      float cos = x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1,
            dir = (cos >= 0 ? 1 : -1),
            sqrSin = 1 - cos * cos;

      // Skip the Slerp for tiny steps to avoid numeric problems:
      float EPSILON = 2.220446049250313e-16;
      if (sqrSin > EPSILON) {

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
      if (s == 1 - t) {

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
  void interpolateFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t, bool isPosition) {
    if (isPosition) {
      lerpFlat(dst, dstOffset, src0, srcOffset0, src1, srcOffset1, t);
    } else {
      slerpFlat(dst, dstOffset, src0, srcOffset0, src1, srcOffset1, t);
    }
  }
  void multiplyQuaternionsFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1) {
    float x0 = src0[ srcOffset0 ];
    float y0 = src0[ srcOffset0 + 1 ];
    float z0 = src0[ srcOffset0 + 2 ];
    float w0 = src0[ srcOffset0 + 3 ];

    float x1 = src1[ srcOffset1 ];
    float y1 = src1[ srcOffset1 + 1 ];
    float z1 = src1[ srcOffset1 + 2 ];
    float w1 = src1[ srcOffset1 + 3 ];

    dst[ dstOffset ] = x0 * w1 + w0 * x1 + y0 * z1 - z0 * y1;
    dst[ dstOffset + 1 ] = y0 * w1 + w0 * y1 + z0 * x1 - x0 * z1;
    dst[ dstOffset + 2 ] = z0 * w1 + w0 * z1 + x0 * y1 - y0 * x1;
    dst[ dstOffset + 3 ] = w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1;
  }
  void invertQuaternionFlat(float *dst, unsigned int dstOffset) {
    dst[ dstOffset ] *= -1;
    dst[ dstOffset + 1 ] *= -1;
    dst[ dstOffset + 2 ] *= -1;
  }
  void addVectorsFlat(float *dst, float *src0, float *src1) {
    dst[0] = src0[0] + src1[0];
    dst[1] = src0[1] + src1[1];
    dst[2] = src0[2] + src1[2];
  }
  void subVectorsFlat(float *dst, float *src0, float *src1) {
    dst[0] = src0[0] - src1[0];
    dst[1] = src0[1] - src1[1];
    dst[2] = src0[2] - src1[2];
  }

  // Main ------

  Avatar *createAnimationAvatar(AnimationMixer *mixer) {
    // std::cout << "createAnimationAvatar ------------------" << std::endl;

    // test
    // for (auto const& x : animationGroups) {
    //   std::map<std::string, Animation *> animationGroup = animationGroups[x.first];
    //   for (auto const& y : animationGroup) {
    //     Animation *animation = animationGroup[y.first];
    //     std::cout << " groupName: " << x.first << " keyName: " << y.first << " name: " << animation->name << std::endl;
    //   }
    // }

    // avatar->mixer = createAnimationMixer();
    Avatar *avatar = new Avatar();
    avatars.push_back(avatar);
    avatar->mixer = mixer;
    mixer->avatar = avatar;

    return avatar;
  }
  void initAnimationSystem(float *scratchStack) { // only need init once globally
    // std::cout << "initAnimationSystem ------------------" << std::endl;

    // -------------------------------------------------------------------------
    
    unsigned int index = 0;

    speedFactors["grab_forward"] = scratchStack[index++];
    speedFactors["grab_down"] = scratchStack[index++];
    speedFactors["grab_up"] = scratchStack[index++];
    speedFactors["grab_left"] = scratchStack[index++];
    speedFactors["grab_right"] = scratchStack[index++];
    speedFactors["pick_up"] = scratchStack[index++];

    defaultSitAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    defaultEmoteAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    defaultDanceAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    defaultHoldAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    defaultActivateAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    defaultNarutoRunAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];

    // -------------------------------------------------------------------------
    
    AnimationName[0] = "";

    // -------------------------------------------------------------------------

    animationGroups["single"]["idle"] = animationAll["idle.fbx"];
    animationGroups["single"]["crouchIdle"] = animationAll["Crouch Idle.fbx"];

    // 8 directions walk animations ---
    animationGroups["walk"]["forward"] = animationAll["walking.fbx"];
    animationGroups["walk"]["backward"] = animationAll["walking backwards.fbx"];
    animationGroups["walk"]["left"] = animationAll["left strafe walking.fbx"];
    animationGroups["walk"]["leftMirror"] = animationAll["right strafe walking reverse.fbx"];
    animationGroups["walk"]["right"] = animationAll["right strafe walking.fbx"];
    animationGroups["walk"]["rightMirror"] = animationAll["left strafe walking reverse.fbx"];

    // 8 directions run animations ---
    animationGroups["run"]["forward"] = animationAll["Fast Run.fbx"];
    animationGroups["run"]["backward"] = animationAll["running backwards.fbx"];
    animationGroups["run"]["left"] = animationAll["left strafe.fbx"];
    animationGroups["run"]["leftMirror"] = animationAll["right strafe reverse.fbx"];
    animationGroups["run"]["right"] = animationAll["right strafe.fbx"];
    animationGroups["run"]["rightMirror"] = animationAll["left strafe reverse.fbx"];

    // 8 directions crouch animations ---
    animationGroups["crouch"]["forward"] = animationAll["Sneaking Forward.fbx"];
    animationGroups["crouch"]["backward"] = animationAll["Sneaking Forward reverse.fbx"];
    animationGroups["crouch"]["left"] = animationAll["Crouched Sneaking Left.fbx"];
    animationGroups["crouch"]["leftMirror"] = animationAll["Crouched Sneaking Right reverse.fbx"];
    animationGroups["crouch"]["right"] = animationAll["Crouched Sneaking Right.fbx"];
    animationGroups["crouch"]["rightMirror"] = animationAll["Crouched Sneaking Left reverse.fbx"];
  }
  void Avatar::update(float *scratchStack) {
    unsigned int index = 0;

    // values ---
    this->forwardFactor = scratchStack[index++];
    this->backwardFactor = scratchStack[index++];
    this->leftFactor = scratchStack[index++];
    this->rightFactor = scratchStack[index++];
    this->mirrorLeftFactorReverse = scratchStack[index++];
    this->mirrorLeftFactor = scratchStack[index++];
    this->mirrorRightFactorReverse = scratchStack[index++];
    this->mirrorRightFactor = scratchStack[index++];

    this->idleWalkFactor = scratchStack[index++];
    this->walkRunFactor = scratchStack[index++];
    this->crouchFactor = scratchStack[index++];

    this->activateTime = scratchStack[index++];

    // action states ---
    this->jumpState = scratchStack[index++];
    this->doubleJumpState = scratchStack[index++];
    this->flyState = scratchStack[index++];
    this->narutoRunState = scratchStack[index++];
    this->sitState = scratchStack[index++];
    this->holdState = scratchStack[index++];
    this->pickUpState = scratchStack[index++];

    // other ---
    this->landWithMoving = scratchStack[index++];
    this->landTime = scratchStack[index++];
    this->fallLoopFactor = scratchStack[index++];
    this->fallLoopTime = scratchStack[index++];
    this->flyTime = scratchStack[index++];
    this->doubleJumpTime = scratchStack[index++];
    this->jumpTime = scratchStack[index++];
    this->narutoRunTime = scratchStack[index++];
    this->narutoRunTimeFactor = scratchStack[index++];
    this->danceFactor = scratchStack[index++];
    this->crouchMaxTime = scratchStack[index++];
    this->emoteFactor = scratchStack[index++];
    this->lastEmoteTime = scratchStack[index++];
    this->useTime = scratchStack[index++];
    this->useAnimationEnvelopeLength = scratchStack[index++];
    this->hurtTime = scratchStack[index++];
    this->unuseTime = scratchStack[index++];
    this->aimTime = scratchStack[index++];
    this->aimMaxTime = scratchStack[index++];
    this->pickUpTime = scratchStack[index++];
    this->useAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->emoteAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->sitAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->danceAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->activateAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->hurtAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->useAnimationComboName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->unuseAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->aimAnimationName = AnimationName[(unsigned int)(scratchStack[index++])];
    this->fallLoopFrom = (unsigned int)(scratchStack[index++]) == 1 ? "jump" : "";
    this->landTimeS = scratchStack[index++];
    this->timeSinceLastMoveS = scratchStack[index++];

    this->useAnimationEnvelopeNames.clear();
    for (unsigned int i = 0; i < useAnimationEnvelopeLength; i++) {
      this->useAnimationEnvelopeNames.push_back(AnimationName[(unsigned int)(scratchStack[index++])]);
    }

    //
    
    localWeights["forward"] = this->forwardFactor;
    localWeights["backward"] = this->backwardFactor;
    localWeights["left"] = this->mirrorLeftFactorReverse;
    localWeights["leftMirror"] = this->mirrorLeftFactor;
    localWeights["right"] = this->mirrorRightFactorReverse;
    localWeights["rightMirror"] = this->mirrorRightFactor;
  }
  AnimationMixer *createAnimationMixer() {
    AnimationMixer *animationMixer = new AnimationMixer();
    _animationMixers.push_back(animationMixer);
    return animationMixer;
  }
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone, bool isTop, bool isArm) {
    AnimationMapping animationMapping;
    animationMapping.isPosition = isPosition;
    animationMapping.index = index;
    animationMapping.isFirstBone = isFirstBone;
    animationMapping.isLastBone = isLastBone;
    animationMapping.isTop = isTop;
    animationMapping.isArm = isArm;
    _animationMappings.push_back(animationMapping);
  }
  Animation *createAnimation(char *scratchStack, unsigned int nameByteLength, float duration) {
    Animation *animation = new Animation();
    animation->duration = duration;
    std::string name = "";
    for (unsigned int i = 0; i < nameByteLength; i++) {
      name += scratchStack[i];
    }
    animation->name = name;

    animationAll[name] = animation;

    return animation;
  }
  void setAnimationGroup(Animation *animation, char *scratchStack, unsigned int groupNameByteLength, unsigned int keyNameByteLength, unsigned int keyNameUInt) {
    
    unsigned int index = 0;

    std::string groupName = "";
    for (unsigned int i = 0; i < groupNameByteLength; i++) {
      groupName += scratchStack[index++];
    }

    std::string keyName = "";
    for (unsigned int i = 0; i < keyNameByteLength; i++) {
      keyName += scratchStack[index++];
    }

    // if (!animationGroups[groupName]) { // note: don't need this check.
    //   animationGroups[groupName] = new std::map<std::string, Animation *>;
    // }

    animationGroups[groupName][keyName] = animation;

    // std::cout << "groupName: " << groupName << " keyName: " << keyName << " name: " << animation->name << std::endl;

    AnimationName[keyNameUInt] = keyName;
  }
  void createAnimationInterpolant(Animation *animation, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize) {
    Interpolant interpolant;
    interpolant.numParameterPositions = numParameterPositions;
    interpolant.parameterPositions = parameterPositions;
    interpolant.resultBuffer = (float *)malloc(valueSize * sizeof(float));
    interpolant.numSampleValues = numSampleValues;
    interpolant.sampleValues = sampleValues;
    interpolant.valueSize = valueSize; // only support 3 (vector) or 4 (quaternion)

    animation->interpolants.push_back(interpolant);
  }
  float *evaluateInterpolant(Animation *animation, unsigned int interpolantIndex, float t) {
    Interpolant interpolant = animation->interpolants[interpolantIndex];

    if (interpolant.numParameterPositions == 1) {
      interpolant.resultBuffer[0] = interpolant.sampleValues[0];
      interpolant.resultBuffer[1] = interpolant.sampleValues[1];
      interpolant.resultBuffer[2] = interpolant.sampleValues[2];
      if (interpolant.valueSize == 4) {
        interpolant.resultBuffer[3] = interpolant.sampleValues[3];
      }
    } else {
      int index = 0;
      for (; index < interpolant.numParameterPositions; index++) {
        if (interpolant.parameterPositions[index] > t) {
          break;
        }
      }

      if (index == 0) { // note: Handle situation that, parameterPositions[0] > 0, and t == 0 or t < parameterPositions[0].
        interpolant.resultBuffer[0] = interpolant.sampleValues[0];
        interpolant.resultBuffer[1] = interpolant.sampleValues[1];
        interpolant.resultBuffer[2] = interpolant.sampleValues[2];
        if (interpolant.valueSize == 4) {
          interpolant.resultBuffer[3] = interpolant.sampleValues[3];
        }
      } else if (index > interpolant.numParameterPositions - 1) { // note: Handle situation that, t > max parameterPosition.
        unsigned int maxIndex = interpolant.numParameterPositions - 1;
        interpolant.resultBuffer[0] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 0];
        interpolant.resultBuffer[1] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 1];
        interpolant.resultBuffer[2] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 2];
        if (interpolant.valueSize == 4) {
          interpolant.resultBuffer[3] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 3];
        }
      } else {
        unsigned int index0 = index - 1;
        unsigned int index1 = index;

        float time0 = interpolant.parameterPositions[index0];
        float time1 = interpolant.parameterPositions[index1];
        float f = (t - time0) / (time1 - time0);

        interpolateFlat(
          interpolant.resultBuffer, 0,
          interpolant.sampleValues, index0 * interpolant.valueSize,
          interpolant.sampleValues, index1 * interpolant.valueSize,
          f,
          interpolant.valueSize == 3
        );
      }
    }

    return interpolant.resultBuffer;
  }

  float *doBlendList(AnimationMapping &spec, std::map<std::string, Animation *> &animations, float &timeS) { // note: Big performance influnce!!! Use `&` to prevent copy parameter's values!!!
    float *resultVecQuat;
    unsigned int indexWeightBigThanZero = 0;
    float currentWeight = 0;
    // for (int i = 0; i < numAnimations; i++) {
    for (auto const& x : animations) {
      // if (spec.isPosition) std::cout << " x.first: " << x.first << std::endl;
      float weight = localWeights[x.first];
      if (weight > 0) {
        Animation *animation = animations[x.first];
        float *vecQuat = evaluateInterpolant(animation, spec.index, fmod(timeS, animation->duration));
        if (indexWeightBigThanZero == 0) {
          resultVecQuat = vecQuat;

          indexWeightBigThanZero++;
          currentWeight = weight;
        } else {
          float t = weight / (currentWeight + weight);
          interpolateFlat(resultVecQuat, 0, resultVecQuat, 0, vecQuat, 0, t, spec.isPosition);

          indexWeightBigThanZero++;
          currentWeight += weight;
        }
      }
    }
    return resultVecQuat;
  }

  void _handleDefault(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_handleDefault, "; // test: blend strings.

    // note: Big performance influnce!!! Do not update `localWeights` here, because of will run 53 times ( 53 bones )!!! todo: Notice codes which will run 53 times!!!
    // localWeights["forward"] = avatar->forwardFactor;
    // localWeights["backward"] = avatar->backwardFactor;
    // localWeights["left"] = avatar->mirrorLeftFactorReverse;
    // localWeights["leftMirror"] = avatar->mirrorLeftFactor;
    // localWeights["right"] = avatar->mirrorRightFactorReverse;
    // localWeights["rightMirror"] = avatar->mirrorRightFactor;

    // walkAnimations
    localVecQuatPtr2 = doBlendList(spec, animationGroups["walk"], avatar->landTimeS);
    copyValue(spec.dst, localVecQuatPtr2, spec.isPosition);

    // if (avatar->walkRunFactor > 0) {
      // runAnimations
      localVecQuatPtr2 = doBlendList(spec, animationGroups["run"], avatar->landTimeS);

      // blend walk run
      interpolateFlat(spec.dst, 0, spec.dst, 0, localVecQuatPtr2, 0, avatar->walkRunFactor, spec.isPosition);
      _clearXZ(spec.dst, spec.isPosition);
    // }

    // blend idle ---
    // if (avatar->idleWalkFactor < 1) {
      localVecQuatPtr = evaluateInterpolant(animationGroups["single"]["idle"], spec.index, fmod(avatar->timeSinceLastMoveS, animationGroups["single"]["idle"]->duration));
      interpolateFlat(spec.dst, 0, spec.dst, 0, localVecQuatPtr, 0, 1 - avatar->idleWalkFactor, spec.isPosition);
    // }

    // if (avatar->crouchFactor > 0) {
      // crouchAnimations
      localVecQuatPtr2 = doBlendList(spec, animationGroups["crouch"], avatar->landTimeS);
      copyValue(localVecQuatArr, localVecQuatPtr2, spec.isPosition);
      _clearXZ(localVecQuatArr, spec.isPosition);

      // blend crouch idle ---
      // if (avatar->idleWalkFactor < 1) {
        localVecQuatPtr = evaluateInterpolant(animationGroups["single"]["crouchIdle"], spec.index, fmod(avatar->timeSinceLastMoveS, animationGroups["single"]["crouchIdle"]->duration));
        interpolateFlat(localVecQuatArr, 0, localVecQuatArr, 0, localVecQuatPtr, 0, 1 - avatar->idleWalkFactor, spec.isPosition);
      // }

      // blend walkRun and crouch
      interpolateFlat(spec.dst, 0, spec.dst, 0, localVecQuatArr, 0, avatar->crouchFactor, spec.isPosition);
    // }
  }

  void _blendDoubleJump(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendDoubleJump, "; // test: blend strings.

    float t2 = avatar->doubleJumpTime / 1000;
    float *v2 = evaluateInterpolant(animationGroups["single"]["doubleJump"], spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }

  void _blendJump(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendJump, "; // test: blend strings.

    float t2 = avatar->jumpTime / 1000;
    float *v2 = evaluateInterpolant(animationGroups["single"]["jump"], spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);

    if (avatar->holdState && spec.isArm) {
      Animation *holdAnimation = animationGroups["hold"][defaultHoldAnimationName];
      float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
      float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);
      copyValue(spec.dst, v2, spec.isPosition);
    }
  }

  void _blendSit(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendSit, "; // test: blend strings.

    Animation *sitAnimation = animationGroups["sit"][avatar->sitAnimationName == "" ? defaultSitAnimationName : avatar->sitAnimationName];
    float *v2 = evaluateInterpolant(sitAnimation, spec.index, 1);

    copyValue(spec.dst, v2, spec.isPosition);
  }

  void _blendNarutoRun(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendNarutoRun, "; // test: blend strings.

    Animation *narutoRunAnimation = animationGroups["narutoRun"][defaultNarutoRunAnimationName];
    float t2 = fmod((avatar->narutoRunTime / 1000 * avatar->narutoRunTimeFactor), narutoRunAnimation->duration);
    float *v2 = evaluateInterpolant(narutoRunAnimation, spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }

  void _blendDance(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendDance, "; // test: blend strings.

    _handleDefault(spec, avatar);

    Animation *danceAnimation = animationGroups["dance"][avatar->danceAnimationName == "" ? defaultDanceAnimationName : avatar->danceAnimationName];
    float t2 = fmod(AnimationMixer::nowS, danceAnimation->duration);
    float *v2 = evaluateInterpolant(danceAnimation, spec.index, t2);

    float danceFactorS = avatar->danceFactor / avatar->crouchMaxTime;
    float f = min(max(danceFactorS, 0), 1);
    
    interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }

  void _blendEmote(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendEmote, "; // test: blend strings.

    _handleDefault(spec, avatar);

    Animation *emoteAnimation = animationGroups["emote"][avatar->emoteAnimationName == "" ? defaultEmoteAnimationName : avatar->emoteAnimationName];
    float emoteTime = AnimationMixer::nowS * 1000 - avatar->lastEmoteTime; // todo: use now.
    float t2 = min(emoteTime / 1000, emoteAnimation->duration);
    float *v2 = evaluateInterpolant(emoteAnimation, spec.index, t2);

    float emoteFactorS = avatar->emoteFactor / avatar->crouchMaxTime;
    float f = min(max(emoteFactorS, 0), 1);

    if (spec.index == BoneIndex::Spine || spec.index == BoneIndex::Chest || spec.index == BoneIndex::UpperChest || spec.index == BoneIndex::Neck || spec.index == BoneIndex::Head) {
      if (!spec.isPosition) {
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      } else {
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }
    } else {
      if (!spec.isTop) {
        f *= (1 - avatar->idleWalkFactor);
      }

      interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
    }

    _clearXZ(spec.dst, spec.isPosition);
  }

  void _blendUse(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendUse, "; // test: blend strings.

    Animation *useAnimation = nullptr;
    float t2;
    float useTimeS = avatar->useTime / 1000;
    if (avatar->useAnimationName != "") {
      useAnimation = animationGroups["use"][avatar->useAnimationName];
      t2 = min(useTimeS, useAnimation->duration);
    } else if(avatar->useAnimationComboName != "") {
      useAnimation = animationGroups["use"][avatar->useAnimationComboName];
      t2 = min(useTimeS, useAnimation->duration);
    } else if (avatar->useAnimationEnvelopeNames.size() > 0) {
      float totalTime = 0;
      for (unsigned int i = 0; i < avatar->useAnimationEnvelopeNames.size() - 1; i++) {
        std::string animationName = avatar->useAnimationEnvelopeNames[i];
        Animation *animation = animationGroups["use"][animationName];
        totalTime += animation->duration;
      }

      if (totalTime > 0) {
        float animationTimeBase = 0;
        for (unsigned int i = 0; i < avatar->useAnimationEnvelopeNames.size() - 1; i++) {
          std::string animationName = avatar->useAnimationEnvelopeNames[i];
          Animation *animation = animationGroups["use"][animationName];
          if (useTimeS < (animationTimeBase + animation->duration)) {
            useAnimation = animation;
            break;
          }
          animationTimeBase += animation->duration;
        }
        if (useAnimation != nullptr) { // first iteration
          t2 = min(useTimeS - animationTimeBase, useAnimation->duration);
        } else { // loop
          std::string secondLastAnimationName = avatar->useAnimationEnvelopeNames[avatar->useAnimationEnvelopeNames.size() - 2];
          useAnimation = animationGroups["use"][secondLastAnimationName];
          t2 = fmod((useTimeS - animationTimeBase), useAnimation->duration);
        }
      }
    }
    
    _handleDefault(spec, avatar);

    if (useAnimation) {
      if (!spec.isPosition) {
        float *v2 = evaluateInterpolant(useAnimation, spec.index, t2);

        Animation *idleAnimation = animationGroups["single"]["idle"];
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      } else {
        float *v2 = evaluateInterpolant(useAnimation, spec.index, t2);
        _clearXZ(v2, spec.isPosition);

        Animation *idleAnimation = animationGroups["single"]["idle"];
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        subVectorsFlat(spec.dst, spec.dst, v3);
        addVectorsFlat(spec.dst, spec.dst, v2);
      }
    }
  }

  void _blendHurt(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendHurt, "; // test: blend strings.

    _handleDefault(spec, avatar);

    Animation *hurtAnimation = animationGroups["hurt"][avatar->hurtAnimationName];
    float hurtTimeS = avatar->hurtTime / 1000;
    float t2 = min(hurtTimeS, hurtAnimation->duration);
    if (!spec.isPosition) {
      if (hurtAnimation) {
        float *v2 = evaluateInterpolant(hurtAnimation, spec.index, t2);

        Animation *idleAnimation = animationGroups["single"]["idle"];
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
        
        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      }
    } else {
      float *v2 = evaluateInterpolant(hurtAnimation, spec.index, t2);

      Animation *idleAnimation = animationGroups["single"]["idle"];
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

      subVectorsFlat(spec.dst, spec.dst, v3);
      addVectorsFlat(spec.dst, spec.dst, v2);
    }
  }

  void _blendAim(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendAim, "; // test: blend strings.
    
    _handleDefault(spec, avatar);

    Animation *aimAnimation = animationGroups["aim"][avatar->aimAnimationName];
    float t2 = fmod((avatar->aimTime / avatar->aimMaxTime), aimAnimation->duration);
    if (!spec.isPosition) {
      if (aimAnimation) {
        float *v2 = evaluateInterpolant(aimAnimation, spec.index, t2);

        Animation *idleAnimation = animationGroups["single"]["idle"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      }
    } else {
      float *v2 = evaluateInterpolant(aimAnimation, spec.index, t2);

      Animation *idleAnimation = animationGroups["single"]["idle"];
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

      subVectorsFlat(spec.dst, spec.dst, v3);
      addVectorsFlat(spec.dst, spec.dst, v2);
    }
  }

  void _blendUnuse(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendUnuse, "; // test: blend strings.
    
    _handleDefault(spec, avatar);

    float unuseTimeS = avatar->unuseTime / 1000;
    Animation *unuseAnimation = animationGroups["use"][avatar->unuseAnimationName];
    float t2 = min(unuseTimeS, unuseAnimation->duration);
    float f = min(max(unuseTimeS / unuseAnimation->duration, 0), 1);
    float f2 = std::pow(1 - f, 2);

    if (!spec.isPosition) {
      float *v2 = evaluateInterpolant(unuseAnimation, spec.index, t2);

      Animation *idleAnimation = animationGroups["single"]["idle"];
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
        
      copyValue(localQuaternionArr, spec.dst, spec.isPosition);
      invertQuaternionFlat(v3, 0);
      multiplyQuaternionsFlat(localQuaternionArr, 0, v3, 0, localQuaternionArr, 0);
      multiplyQuaternionsFlat(localQuaternionArr, 0, v2, 0, localQuaternionArr, 0);

      interpolateFlat(spec.dst, 0, spec.dst, 0, localQuaternionArr, 0, f2, spec.isPosition);
    } else {
      float *v2 = evaluateInterpolant(unuseAnimation, spec.index, t2);

      Animation *idleAnimation = animationGroups["single"]["idle"];
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
      
      copyValue(localVectorArr, spec.dst, spec.isPosition);
      subVectorsFlat(localVectorArr, localVectorArr, v3);
      addVectorsFlat(localVectorArr, localVectorArr, v2);

      interpolateFlat(spec.dst, 0, spec.dst, 0, localVectorArr, 0, f2, spec.isPosition);
    }
  }

  void _blendHold(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendHold, "; // test: blend strings.
    
    _handleDefault(spec, avatar);

    Animation *holdAnimation = animationGroups["hold"][defaultHoldAnimationName];
    float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
    float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);

    if (spec.isTop) {
      if (spec.index == BoneIndex::Left_arm || spec.index == BoneIndex::Right_arm) {
        copyValue(spec.dst, v2, spec.isPosition);
      } else {
        if (spec.isArm) {
          float f = avatar->walkRunFactor * 0.7 + avatar->crouchFactor * (1 - avatar->idleWalkFactor) * 0.5;
          interpolateFlat(spec.dst, 0, spec.dst, 0, identityQuaternion, 0, f, spec.isPosition);
          multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
        } else {
          multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
        }
      }
    }
  }

  void _blendPickUp(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendPickUp, "; // test: blend strings.

    Animation *pickUpAnimation = animationGroups["pickup"]["pickUpZelda"];
    Animation *pickUpIdleAnimation = animationGroups["pickup"]["pickUpIdleZelda"];

    float t2 = avatar->pickUpTime / 1000;
    if (t2 < pickUpAnimation->duration) {
      float *v2 = evaluateInterpolant(pickUpAnimation, spec.index, t2);
      copyValue(spec.dst, v2, spec.isPosition);
    } else {
      float t3 = fmod((t2 - pickUpAnimation->duration), pickUpIdleAnimation->duration);
      float *v2 = evaluateInterpolant(pickUpIdleAnimation, spec.index, t3);
      copyValue(spec.dst, v2, spec.isPosition);
    }
  }

  void _blendFly(AnimationMapping &spec, Avatar *avatar) {
    if (avatar->flyState || (avatar->flyTime >= 0 && avatar->flyTime < 1000)) {
      // if (spec.isPosition) avatar->testBlendStrings += "_blendFly, "; // test: blend strings.

      float t2 = avatar->flyTime / 1000;
      // const f = avatar->flyState ? min(cubicBezier(t2), 1) : (1 - min(cubicBezier(t2), 1)); // todo: cubicBezier.
      float f = avatar->flyState ? min(pow(t2, 0.1), 1) : (1 - min(pow(t2, 0.1), 1));
      float *v2 = evaluateInterpolant(animationGroups["single"]["float"], spec.index, fmod(t2, animationGroups["single"]["float"]->duration));

      interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);

      if (avatar->holdState && spec.isArm) {
        Animation *holdAnimation = animationGroups["hold"][defaultHoldAnimationName];
        float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
        float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);
        copyValue(spec.dst, v2, spec.isPosition);
      }
    }
  };

  void _blendLand(AnimationMapping &spec, Avatar *avatar) {
    if (!avatar->landWithMoving) {
      float animationSpeed = 0.75;
      float landTimeS = avatar->landTime / 1000;
      Animation *landingAnimation = animationGroups["land"]["landing"];
      float landingAnimationDuration = landingAnimation->duration / animationSpeed;
      float landFactor = landTimeS / landingAnimationDuration;

      if (landFactor > 0 && landFactor <= 1) {
        // if (spec.isPosition) avatar->testBlendStrings += "_blendLand, "; // test: blend strings.

        float t2 = landTimeS * animationSpeed;
        float *v2 = evaluateInterpolant(landingAnimation, spec.index, t2);

        float f = (landingAnimationDuration - landTimeS) / 0.05; // 0.05 = 3 frames
        f = clamp(f, 0, 1);

        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
        _clearXZ(spec.dst, spec.isPosition);
      }
    } else {
      float animationSpeed = 0.95;
      float landTimeS = avatar->landTime / 1000;
      Animation *landingAnimation = animationGroups["land"]["landing2"];
      float landingAnimationDuration = landingAnimation->duration / animationSpeed;
      float landFactor = landTimeS / landingAnimationDuration;

      if (landFactor > 0 && landFactor <= 1) {
        // if (spec.isPosition) avatar->testBlendStrings += "_blendLand, "; // test: blend strings.

        float t2 = landTimeS * animationSpeed;
        float *v2 = evaluateInterpolant(landingAnimation, spec.index, t2);

        /* note: Calculating the time since the player landed on the ground. */
        float f3 = landTimeS / 0.1;
        f3 = clamp(f3, 0, 1);

        /* note: Calculating the time remaining until the landing animation is complete. */
        float f2 = (landingAnimationDuration - landTimeS) / 0.15;
        f2 = clamp(f2, 0, 1);

        float f = min(f3, f2);

        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
        _clearXZ(spec.dst, spec.isPosition);
      }
    }
  }

  void _blendFallLoop(AnimationMapping &spec, Avatar *avatar) {
    if (avatar->fallLoopFactor > 0) {
      // if (spec.isPosition) avatar->testBlendStrings += "_blendFallLoop, "; // test: blend strings.

      float t2 = (avatar->fallLoopTime / 1000);
      float *v2 = evaluateInterpolant(animationGroups["single"]["fallLoop"], spec.index, t2);
      float f = clamp(t2 / 0.3, 0, 1);

      if (avatar->fallLoopFrom == "jump") {
        copyValue(spec.dst, v2, spec.isPosition);
      } else {
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }

      _clearXZ(spec.dst, spec.isPosition);
    }
  }

  void _blendActivate(AnimationMapping &spec, Avatar *avatar) {
    if (avatar->activateTime > 0) {
      // if (spec.isPosition) avatar->testBlendStrings += "_blendActivate, "; // test: blend strings.

      std::string activateAnimationName = avatar->activateAnimationName == "" ? defaultActivateAnimationName : avatar->activateAnimationName;
      Animation *activateAnimation = animationGroups["activate"][activateAnimationName];
      float t2 = fmod((avatar->activateTime / 1000 * speedFactors[activateAnimationName]), activateAnimation->duration);
      float *v2 = evaluateInterpolant(activateAnimation, spec.index, t2);

      // const f = avatar.activateTime > 0 ? Math.min(cubicBezier(t2), 1) : (1 - Math.min(cubicBezier(t2), 1));
      float f = avatar->activateTime > 0 ? min(pow(t2, 0.1), 1) : (1 - min(pow(t2, 0.1), 1));

      if (spec.index == BoneIndex::Spine || spec.index == BoneIndex::Chest || spec.index == BoneIndex::UpperChest || spec.index == BoneIndex::Neck || spec.index == BoneIndex::Head) {
        if (!spec.isPosition) {
          multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
        } else {
          interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
        }
      } else {
        if (!spec.isTop) {
          f *= (1 - avatar->idleWalkFactor);
        }
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }
    }
  }

  float **AnimationMixer::update(float now, float nowS) {
    // AnimationMixer::now = now; // why can't set, cause idle and dance animatios play very fast ? use file variale instead ?
    AnimationMixer::nowS = nowS;

    for (int i = 0; i < 53; i++) {
      AnimationMapping spec = _animationMappings[i];

      // if (spec.isPosition) avatar->testBlendStrings = ""; // test: blend strings.

      // note: Use exaclty same early return logic as js version, instead of all cascading, to prevent some bugs. But still want to use all cascading afterwards.
      if (avatar->doubleJumpState) {
        _blendDoubleJump(spec, this->avatar);
      } else if (avatar->jumpState) {
        _blendJump(spec, this->avatar);
      } else if (avatar->sitState) {
        _blendSit(spec, this->avatar);
      } else if (avatar->narutoRunState) {
        _blendNarutoRun(spec, this->avatar);
      } else if (avatar->danceFactor > 0) {
        _blendDance(spec, this->avatar);
      } else if (avatar->emoteFactor > 0) {
        _blendEmote(spec, this->avatar);
      } else if (
        avatar->useAnimationName != "" ||
        avatar->useAnimationComboName != "" ||
        avatar->useAnimationEnvelopeNames.size() > 0
      ) {
        _blendUse(spec, this->avatar);
      } else if (avatar->hurtAnimationName != "") {
        _blendHurt(spec, this->avatar); // todo: move to highest priority.
      } else if (avatar->aimAnimationName != "") {
        _blendAim(spec, this->avatar);
      } else if (avatar->unuseAnimationName != "" && avatar->unuseTime >= 0) {
        _blendUnuse(spec, this->avatar);
      } else if (avatar->holdState) {
        _blendHold(spec, this->avatar);
      } else if (avatar->pickUpState) {
        _blendPickUp(spec, this->avatar);
      } else {
        _handleDefault(spec, this->avatar);
      }

      // note: cascading blending, in order to do transition between all kinds of aniamtions.
      _blendFly(spec, this->avatar);
      _blendFallLoop(spec, this->avatar);
      _blendLand(spec, this->avatar);
      _blendActivate(spec, this->avatar);

      // if (spec.isPosition) std::cout << "testBlendStrings: " << avatar->testBlendStrings << std::endl; // test: blend strings.

      // animationValues[i][0] = spec.dst[0];
      // animationValues[i][1] = spec.dst[1];
      // animationValues[i][2] = spec.dst[2];
      // if (!spec.isPosition) animationValues[i][3] = spec.dst[3];
      animationValues[i] = spec.dst;
    }

    return animationValues;
  }
}