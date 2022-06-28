#ifndef _ANIMATIONSYSTEM_H
#define _ANIMATIONSYSTEM_H
#include <iostream>
#include "vector.h"

namespace AnimationSystem
{
  struct Interpolant
  {
    unsigned int numParameterPositions;
    float *parameterPositions;
    float *resultBuffer;
    unsigned int numSampleValues;
    float *sampleValues;
    unsigned int valueSize;
  };
  struct Animation
  {
    float duration;
    std::vector<Interpolant> interpolants; // todo: pure array?
    unsigned int currentInterpolantIndex = 0;
    unsigned int index;
    float weight = 1; // todo: move to AnimationMotion
  };
  struct AnimationMapping
  {
    bool isPosition;
    unsigned int index;
    bool isFirstBone;
    bool isLastBone;
  };
  // class AnimationMotion {
  // public:

  // }
  class AnimationNode
  {
  public:
    std::vector<std::reference_wrapper<Animation>> children;

    float *update(AnimationMapping &spec);
  };
  class AnimationMixer
  {
  public:
    static float timeS;

    AnimationNode _animationNode; // todo: rename: animationTree
    unsigned int _avatarId;
    AnimationMixer(unsigned int avatarId, Animation animation)
    {
      // _animation = animation;
      _avatarId = avatarId;
    };
    // createMotion(Animation animation) {

    // }
    float **update(float timeS, float f);
  };

  float setTest(float num);
  float setTest2(float num);
  float setTestNew(float num);
  float setTestAlloc(float num);
  float getTest();
  //
  float setWeight1(float weight);
  float setWeight2(float weight);
  float getWeight();
  // ------
  // AnimationMixer *createAnimationMixer(unsigned int avatarId);
  void createAnimationMixer(unsigned int avatarId);
  float **updateAnimationMixer(float timeS, float f);
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone);
  // float createAnimation();
  void createAnimation(float duration);
  void createInterpolant(unsigned int animationIndex, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  float *evaluateInterpolant(unsigned int animationIndex, unsigned int interpolantIndex, float t);
  float **getAnimationValues(unsigned int animationIndex, float t);
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  float changeWeight(unsigned int animationIndex, float weight);

};

#endif // _ANIMATIONSYSTEM_H
