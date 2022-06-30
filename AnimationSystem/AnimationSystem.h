#ifndef _ANIMATIONSYSTEM_H
#define _ANIMATIONSYSTEM_H
#include <iostream>
#include "vector.h"

namespace AnimationSystem
{
  enum NodeType
  {
    LIST = 1,
    TWO = 2,
    UNITARY = 3,
    OVERWRITE = 4
  };
  enum LoopType
  {
    LoopOnce = 2200,
    LoopRepeat = 2201,
    LoopPingPong = 2202
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
    // node & motion ------
    float *update(AnimationMapping &spec);

    // node ------
    float weight = 1;
    std::vector<AnimationNode *> children;
    unsigned int type = NodeType::LIST; // todo: NodeType type =.
    bool isCrossFade = false;
    float crossFadeDuration = 0;
    float crossFadeStartTime;
    // NodeType::LIST ---
    // NodeType::TWO ---
    float factor = 0; // [0, 1]
    float crossFadeTargetFactor;
    void crossFadeTwo(float duration, float factor);
    // NodeType::UNITARY ---
    AnimationNode *activeNode;
    void crossFadeUnitary(float duration, AnimationNode *targetNode);
    // NodeType::OVERWRITE ---

    // motion ------
    Animation *animation;
    float startTime;
    float timeBias = 0;
    float speed = 1;
    bool isFinished = false;
    // default values same as THREE.AnimationAction.
    unsigned int loop = LoopType::LoopRepeat; // todo: LoopType loop =.
    bool clampWhenFinished = false;
    //
    void play();
    void stop();
    void setTimeBias(float timeBias);
    void setSpeed(float speed);
    void setLoop(LoopType loopType);
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
  AnimationNode *createNode(NodeType type = NodeType::LIST);
  void addChild(AnimationNode *parent, AnimationNode *child);
  void setAnimTree(AnimationNode *node);
  void createInterpolant(unsigned int animationIndex, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  float *evaluateInterpolant(unsigned int animationIndex, unsigned int interpolantIndex, float t);
  float **getAnimationValues(unsigned int animationIndex, float t);
  // void crossFade(AnimationNode *parentNode, float duration, AnimationNode *targetNode);
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void changeWeight(AnimationNode *node, float weight);
  void changeFactor(AnimationNode *node, float factor);
  float getWeight(AnimationSystem::AnimationNode *node);
  float getFactor(AnimationSystem::AnimationNode *node);

};

#endif // _ANIMATIONSYSTEM_H
