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
  class AnimationNode
  {
  public:
    // node ---
    float weight = 1;
    std::vector<AnimationNode *> children;
    // motion ---
    Animation *animation;

    float *update(AnimationMapping &spec);
  };
  class AnimationMixer
  {
  public:
    static float timeS;

    AnimationNode _animationNode; // todo: rename: animationTree
    unsigned int _avatarId;
    AnimationMixer(unsigned int avatarId)
    {
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
  // ------
  // AnimationMixer *createAnimationMixer(unsigned int avatarId);
  void createAnimationMixer(unsigned int avatarId);
  float **updateAnimationMixer(float timeS, float f);
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone);
  // float createAnimation();
  Animation *createAnimation(float duration);
  AnimationNode *createMotion(Animation *animation);
  AnimationNode *createNode();
  void addChild(AnimationNode *parent, AnimationNode *child);
  void setAnimTree(AnimationNode *node);
  void createInterpolant(unsigned int animationIndex, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  float *evaluateInterpolant(unsigned int animationIndex, unsigned int interpolantIndex, float t);
  float **getAnimationValues(unsigned int animationIndex, float t);
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  float changeWeight(AnimationNode *node, float weight);

};

#endif // _ANIMATIONSYSTEM_H
