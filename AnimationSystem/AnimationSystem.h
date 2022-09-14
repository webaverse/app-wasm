#ifndef _ANIMATIONSYSTEM_H
#define _ANIMATIONSYSTEM_H
#include "physics.h"
#include <iostream>

namespace AnimationSystem
{
  struct Interpolant;
  struct Animation;
  struct AnimationMapping;
  struct Avatar;
  class AnimationNode;
  class AnimationMixer;


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
  enum BoneName
  {
    // Hips = 0, // position
    Hips = 1, // quaternion
    Spine = 2,
    Chest = 3,
    UpperChest = 4,
    Neck = 5,
    Head = 6,
    Left_shoulder = 7,
    Left_arm = 8,
    Left_elbow = 9,
    Left_wrist = 10,
    Left_middleFinger1 = 11,
    Left_middleFinger2 = 12,
    Left_middleFinger3 = 13,
    Left_thumb0 = 14,
    Left_thumb1 = 15,
    Left_thumb2 = 16,
    Left_indexFinger1 = 17,
    Left_indexFinger2 = 18,
    Left_indexFinger3 = 19,
    Left_ringFinger1 = 20,
    Left_ringFinger2 = 21,
    Left_ringFinger3 = 22,
    Left_littleFinger1 = 23,
    Left_littleFinger2 = 24,
    Left_littleFinger3 = 25,
    Right_shoulder = 26,
    Right_arm = 27,
    Right_elbow = 28,
    Right_wrist = 29,
    Right_middleFinger1 = 30,
    Right_middleFinger2 = 31,
    Right_middleFinger3 = 32,
    Right_thumb0 = 33,
    Right_thumb1 = 34,
    Right_thumb2 = 35,
    Right_indexFinger1 = 36,
    Right_indexFinger2 = 37,
    Right_indexFinger3 = 38,
    Right_ringFinger1 = 39,
    Right_ringFinger2 = 40,
    Right_ringFinger3 = 41,
    Right_littleFinger1 = 42,
    Right_littleFinger2 = 43,
    Right_littleFinger3 = 44,
    Right_leg = 45,
    Right_knee = 46,
    Right_ankle = 47,
    Right_toe = 48,
    Left_leg = 49,
    Left_knee = 50,
    Left_ankle = 51,
    Left_toe = 52
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
    std::string name;
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

  class Avatar
  {
  public:
    AnimationMixer *mixer;
    std::map<std::string, AnimationNode *> motiono; // todo: duplicated with mixer.motiono ?
    std::map<std::string, AnimationNode *> useMotiono;
    std::map<std::string, AnimationNode *> useComboMotiono;
    std::map<std::string, AnimationNode *> bowMotiono;
    std::map<std::string, AnimationNode *> sitMotiono;
    std::map<std::string, AnimationNode *> emoteMotiono;
    std::map<std::string, AnimationNode *> danceMotiono;
    std::map<std::string, AnimationNode *> holdMotiono;
    std::map<std::string, AnimationNode *> activateMotiono;
    std::map<std::string, AnimationNode *> hurtMotiono;

    std::map<std::string, AnimationNode *> nodeo; // todo: duplicated with mixer.nodeo ?
    std::vector<std::string> strings;

    // action start/end events
    bool jumpStart, jumpEnd, lastJumpState;
    bool doubleJumpStart, doubleJumpEnd, lastDoubleJumpState;
    bool landStart, landEnd, lastLandState;
    bool flyStart, flyEnd, lastFlyState;
    bool activateStart, activateEnd, lastActivateState;
    bool narutoRunStart, narutoRunEnd, lastNarutoRunState;
    bool sitStart, sitEnd, lastSitState;
    bool holdStart, holdEnd, lastHoldState;
    bool emoteStart, emoteEnd, lastEmoteState;
    bool fallLoopStart, fallLoopEnd, lastFallLoopState;
    bool hurtStart, hurtEnd, lastHurtState;
    bool danceStart, danceEnd, lastDanceState;
    bool useEnvelopeStart, useEnvelopeEnd, lastUseEnvelopeState;
    //
    bool useStart, useEnd;
    std::string lastUseAnimation;
    //
    bool useComboStart, useComboEnd;
    std::string lastUseAnimationComboName;
    
    void createMotions();
    void createNodes();
    void update(float *scratchStack);
    void updateString(char *scratchStack, unsigned int numStrings);
  };
  class AnimationNode
  {
  public:
    // node & motion ------
    AnimationMixer *mixer;
    float *results[53];
    float *update(AnimationMapping &spec);
    std::string name;

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

    // node ------
    float weight = 1;
    std::vector<AnimationNode *> children;
    unsigned int type = NodeType::LIST; // todo: NodeType type =.
    bool isCrossFade = false;
    float crossFadeDuration = 0;
    float crossFadeStartTime;
    void setWeight(float weight);
    void setFactor(float factor);
    void setArg(float arg);
    float getWeight();
    float getFactor();
    void addChild(AnimationNode *child);
    unsigned int getChildren(AnimationNode **scratchStack);

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
  };
  class AnimationMixer // note: mixer can't aware of avatar.
  {
  public:
    static float timeS;

    // unsigned int index;
    AnimationNode _animationNode; // todo: rename: animationTree
    AnimationNode *rootNode;
    // std::vector<AnimationNode *> motions;
    std::map<std::string, AnimationNode *> motiono;
    std::map<std::string, AnimationNode *> nodeo;
    float finishedFlag = 0;
    // float *finishedFlag = (float *)malloc((1) * sizeof(float));
    // float *finishedFlag = new float();
    // float *animationValues[55]; // 53 bones interpolants result buffers + 1 finished event flag + 1 finished motion pointer.
    float *animationValues[54]; // 53 bones interpolants result buffers + 1 finished event flag.
    AnimationNode *finishedMotion; // todo: support multiple motions.

    AnimationMixer()
    {
      animationValues[53] = &finishedFlag;
    }

    AnimationNode *createMotion(Animation *animation, std::string name = "");
    AnimationNode *getMotion(char *scratchStack, unsigned int nameByteLength);
    unsigned int getMotionName(AnimationNode *motion, char *scratchStack);
    AnimationNode *createNode(NodeType type = NodeType::LIST, std::string name = "", unsigned int index = 0);
    AnimationNode *getNode(char *scratchStack, unsigned int nameByteLength);
    void setRootNode(AnimationNode *node);
    void plotNodeTree(AnimationNode *node, unsigned int indent = 0);
    void getNodeTreeData(AnimationNode *node);
    float **update(float timeS);
    unsigned int getFinishedMotionName(char *scratchStack);
  };

  // ------
  Avatar *createAvatar(AnimationMixer *mixer); // todo: rename: createAvatar()
  AnimationMixer *createAnimationMixer();
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone, bool isTop, bool isArm);
  // float createAnimation();
  Animation *createAnimation(char *scratchStack, unsigned int nameByteLength, float duration);
  Animation *getAnimation(char *scratchStack, unsigned int nameByteLength);
  void createInterpolant(char *scratchStack, unsigned int animationNameByteLength, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  // float *evaluateInterpolant(unsigned int animationIndex, unsigned int interpolantIndex, float t);
  // float **getAnimationValues(unsigned int animationIndex, float t);
  // void crossFade(AnimationNode *parentNode, float duration, AnimationNode *targetNode);
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t);

};

#endif // _ANIMATIONSYSTEM_H
