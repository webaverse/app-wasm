#ifndef _ANIMATIONSYSTEM_H
#define _ANIMATIONSYSTEM_H
#include <iostream>
#include "vector.h"
#include <optional>

namespace AnimationSystem
{
  struct AnimationMapping
  {
    bool isPosition;
    unsigned int index;
    bool isFirstBone;
    bool isLastBone;
  };
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
  class AnimationMotion
  {
  public:
    Animation animation;
    float timeBias = 0;
    float speed = 1;
    float weight = 1;
    float startTime = 0; // todo: rename playStartTime

    float *update(AnimationMapping spec);
  };
  class AnimationNode
  {
  public:
    // AnimationMixer mixer;
    std::vector<AnimationMotion> children;
    float fadeStartTime;
    unsigned int index;

    float *doBlendList(AnimationMapping spec);
    float *update(AnimationMapping spec);
  };
  class AnimationMixer
  {
  public:
    static float timeS;

    unsigned int _avatarId;
    // AnimationNode animTree;

    AnimationMixer(unsigned int avatarId)
    {
      _avatarId = avatarId;
    };

    // createMotion(Animation animation) {

    // }
    float *doBlend(AnimationNode node, AnimationMapping spec);
    float **update(float timeS, float f);
  };

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

};

#endif // _ANIMATIONSYSTEM_H
