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
    std::vector<Interpolant> interpolants; // todo: pure array?
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
    // NodeType::OVERWRITE ---
    float factor = 0; // [0, 1]
    float crossFadeTargetFactor;
    void crossFadeTwo(float duration, float factor);

    // NodeType::UNITARY ---
    AnimationNode *activeNode;
    void crossFadeUnitary(float duration, AnimationNode *targetNode);

    // NodeType::FUNC ---
    float arg = 0;
    // factor

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

    // unsigned int index;
    AnimationNode _animationNode; // todo: rename: animationTree
    AnimationNode *rootNode;
    std::vector<AnimationNode *> motions;
    float finishedFlag = 0;
    // float *finishedFlag = (float *)malloc((1) * sizeof(float));
    // float *finishedFlag = new float();
    float *animationValues[55]; // 53 bones interpolants result buffers + 1 finished event flag + 1 finished motion pointer.
    // test ---
    float testMixerFloat = 7;
    float *testMixerPointer;
    float **testMixerPointerPointer;

    AnimationMixer()
    {
      animationValues[53] = &finishedFlag;
      // test ---
      testMixerPointer = &testMixerFloat;
      testMixerPointerPointer = &testMixerPointer;
    }

    AnimationNode *createMotion(Animation *animation);
    AnimationNode *createNode(NodeType type = NodeType::LIST);
    float **update(float timeS);
    // test ---
    float getTestMixerFloat()
    {
      return testMixerFloat;
    }
    float *getTestMixerPointer()
    {
      return testMixerPointer;
    }
    float **getTestMixerPointerPointer()
    {
      return testMixerPointerPointer;
    }
    void setTestMixerFloat(float val)
    {
      testMixerFloat = val;
    }
    //
    float getFinishedFlag()
    {
      return finishedFlag;
    }
    float *getFinishedFlagPointer()
    {
      return animationValues[53];
    }
    float **getFinishedFlagPointerPointer()
    {
      return animationValues;
    }
    void setFinishedFlag(float val)
    {
      finishedFlag = val;
    }
    // end test ---
  };

  float setTest(float num);
  float setTest2(float num);
  float setTestNew(float num);
  float setTestAlloc(float num);
  float getTest();
  // ------
  AnimationMixer *createAnimationMixer();
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone, bool isTop, bool isArm);
  // float createAnimation();
  Animation *createAnimation(float duration);
  // test ---
  Animation *getAnimation1(unsigned int index);
  float *getAnimation2(unsigned int index);
  unsigned int *getAnimation3(unsigned int index);
  float **getAnimation4(unsigned int index);
  unsigned int **getAnimation5(unsigned int index);
  Animation **getAnimation6(unsigned int index);
  Animation **getAnimation7(unsigned int index);
  float getFloat1();
  float *getFloat2();
  float **getFloat3();
  Animation getAnimationB1();
  Animation *getAnimationB2();
  Animation **getAnimationB3();
  float **getAnimationB4();
  unsigned int **getAnimationB5();
  // end test ---
  void addChild(AnimationNode *parent, AnimationNode *child);
  void setRootNode(AnimationMixer *mixer, AnimationNode *node);
  void createInterpolant(unsigned int animationIndex, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  // float *evaluateInterpolant(unsigned int animationIndex, unsigned int interpolantIndex, float t);
  // float **getAnimationValues(unsigned int animationIndex, float t);
  // void crossFade(AnimationNode *parentNode, float duration, AnimationNode *targetNode);
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void setWeight(AnimationNode *node, float weight);
  void setFactor(AnimationNode *node, float factor);
  void setArg(AnimationNode *node, float arg);
  float getWeight(AnimationSystem::AnimationNode *node);
  float getFactor(AnimationSystem::AnimationNode *node); // todo: AnimationSystem::AnimationNode -> AnimationNode.
  unsigned int getChildren(AnimationSystem::AnimationNode *node, AnimationSystem::AnimationNode **scratchStack);

};

#endif // _ANIMATIONSYSTEM_H
