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
    SOLITARY = 3,
    OVERWRITE = 4,
    FUNC = 5
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
    std::vector<Interpolant> interpolants;
    unsigned int currentInterpolantIndex = 0;
    unsigned int index;
  };
  struct AnimationMapping // spec
  {
    bool isPosition;
    unsigned int index;
    bool isFirstBone;
    bool isLastBone;
    bool isTop;
    bool isArm;
  };
  class AnimationMixer;
  class AnimationNode
  {
  public:
    // node & motion ------
    AnimationMixer *mixer;
    float *results[53];
    float *update(AnimationMapping &spec);

    // node ------
    float weight = 1;
    std::vector<AnimationNode *> children;
    unsigned int type = NodeType::LIST;
    bool isCrossFade = false;
    float crossFadeDuration = 0;
    float crossFadeStartTime;

    // NodeType::LIST ---

    // NodeType::TWO ---
    // NodeType::OVERWRITE ---
    float factor = 0; // [0, 1]
    float crossFadeTargetFactor;
    void crossFadeTwo(float duration, float factor);

    // NodeType::SOLITARY ---
    AnimationNode *activeNode;
    void crossFadeSolitary(float duration, AnimationNode *targetNode);

    // NodeType::FUNC ---
    unsigned int index;
    float arg = 0;
    // factor
    // void crossFadeTwo(float duration, float factor);

    // motion ------
    Animation *animation;
    float startTime;
    float timeBias = 0;
    float speed = 1;
    bool isFinished = false;
    // default values same as THREE.AnimationAction.
    unsigned int loop = LoopType::LoopRepeat;
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

    AnimationNode _animationNode;
    AnimationNode *rootNode;
    std::vector<AnimationNode *> motions;
    float finishedFlag = 0;
    float *animationValues[55]; // 53 bones interpolants result buffers + 1 finished event flag + 1 finished motion pointer.

    AnimationMixer()
    {
      animationValues[53] = &finishedFlag;
    }

    AnimationNode *createMotion(Animation *animation);
    AnimationNode *createNode(NodeType type = NodeType::LIST, unsigned int index = 0);
    float **update(float timeS);
  };

  AnimationMixer *createAnimationMixer();
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone, bool isTop, bool isArm);
  Animation *createAnimation(float duration);
  void addChild(AnimationNode *parent, AnimationNode *child);
  void setRootNode(AnimationMixer *mixer, AnimationNode *node);
  void createInterpolant(unsigned int animationIndex, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void setWeight(AnimationNode *node, float weight);
  void setFactor(AnimationNode *node, float factor);
  void setArg(AnimationNode *node, float arg);
  float getWeight(AnimationSystem::AnimationNode *node);
  float getFactor(AnimationSystem::AnimationNode *node);
  unsigned int getChildren(AnimationSystem::AnimationNode *node, AnimationSystem::AnimationNode **scratchStack);

};

#endif // _ANIMATIONSYSTEM_H
