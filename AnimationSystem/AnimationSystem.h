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
  class AnimationMixer
  {
  public:
    Animation _animation; // todo: animationTree
    unsigned int _avatarId;
    AnimationMixer(unsigned int avatarId, Animation animation)
    {
      _animation = animation;
      _avatarId = avatarId;
    };
    // createMotion(Animation animation) {

    // }
    float **update(float timeS);
  };

  // AnimationMixer *createAnimationMixer(unsigned int avatarId);
  void createAnimationMixer(unsigned int avatarId);
  float **updateAnimationMixer(float timeS);
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone);
  // float createAnimation();
  void createAnimation();
  void createInterpolant(unsigned int animationIndex, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  float *evaluateInterpolant(unsigned int animationIndex, unsigned int interpolantIndex, float t);
  float **getAnimationValues(unsigned int animationIndex, float t);
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);

};

#endif // _ANIMATIONSYSTEM_H
