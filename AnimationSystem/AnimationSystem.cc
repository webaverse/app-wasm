#include "AnimationSystem.h"

namespace AnimationSystem
{
  // std::map<std::string, Interpolant *> interpolants;
  // std::vector<Interpolant> _interpolants;
  std::vector<AnimationMixer *> _animationMixers;
  std::vector<AnimationMapping> _animationMappings;
  // std::vector<Animation *> _animations;
  std::map<std::string, Animation *> animationo;
  // float finishedFlag = 0;
  // std::vector<float> _finishedFlags;
  // float _finishedFlags[100];
  // float finishedAnimationIndex; // todo: use pointer instead of index.
  // float **animationValues = (float **)malloc(53 * sizeof(float)); // ok too
  // Interpolant _interpolant;

  float *localQuaternion = (float *)malloc(5 * sizeof(float));

  Animation testAnimation = Animation();
  Animation *testAnimation2 = &testAnimation;
  Animation **testAnimation3 = &testAnimation2;

  float identityQuaternion[4] = {0, 0, 0, 1};

  Avatar avatar; // todo: support multi avatars

  // functions:

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

  // float setWeight(unsigned int animationIndex, float weight)
  // {
  //   // // no effect if _animations not store pointer.
  //   // Animation animation = _animations[animationIndex];
  //   // Animation animation = rootNode.children[animationIndex];
  //   // animation.weight = weight;

  //   rootNode->children[animationIndex]->weight = weight; // todo: test: animationIndex is 0 | 1 here, not real animationIndex.
  //   return weight;
  // }

  void setWeight(AnimationNode *node, float weight)
  {
    // if (node == NULL) std::cout << "no node" << std::endl;
    // if (node->weight == NULL) std::cout << "no weight" << std::endl;
    node->weight = weight;
  }

  void setFactor(AnimationNode *node, float factor)
  {
    node->factor = factor;
  }

  void setArg(AnimationNode *node, float arg)
  {
    node->arg = arg;
  }

  float getWeight(AnimationSystem::AnimationNode *node)
  {
    return node->weight;
  }

  float getFactor(AnimationSystem::AnimationNode *node)
  {
    return node->factor;
  }

  unsigned int getChildren(AnimationNode *node, AnimationNode **scratchStack) {
    unsigned int count = node->children.size();
    for (unsigned int i = 0; i < count; i++) {
      scratchStack[i] = node->children[i];
    }
    return count;
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

  void initAvatar(AnimationMixer *mixer) {
    // avatar.mixer = createAnimationMixer();
    avatar.mixer = mixer;

    // create motion ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    avatar.motiono["idle"] = avatar.mixer->createMotion(animationo["idle.fbx"]);

    avatar.motiono["walkForward"] = avatar.mixer->createMotion(animationo["walking.fbx"]);
    avatar.motiono["walkBackward"] = avatar.mixer->createMotion(animationo["walking backwards.fbx"]);
    avatar.motiono["walkLeft"] = avatar.mixer->createMotion(animationo["left strafe walking.fbx"]);
    avatar.motiono["walkRight"] = avatar.mixer->createMotion(animationo["right strafe walking.fbx"]);
    avatar.motiono["walkLeftMirror"] = avatar.mixer->createMotion(animationo["right strafe walking reverse.fbx"]);
    avatar.motiono["walkRightMirror"] = avatar.mixer->createMotion(animationo["left strafe walking reverse.fbx"]);

    avatar.motiono["runForward"] = avatar.mixer->createMotion(animationo["Fast Run.fbx"]);
    avatar.motiono["runBackward"] = avatar.mixer->createMotion(animationo["running backwards.fbx"]);
    avatar.motiono["runLeft"] = avatar.mixer->createMotion(animationo["left strafe.fbx"]);
    avatar.motiono["runRight"] = avatar.mixer->createMotion(animationo["right strafe.fbx"]);
    avatar.motiono["runLeftMirror"] = avatar.mixer->createMotion(animationo["right strafe reverse.fbx"]);
    avatar.motiono["runRightMirror"] = avatar.mixer->createMotion(animationo["left strafe reverse.fbx"]);

    avatar.motiono["crouchForward"] = avatar.mixer->createMotion(animationo["Sneaking Forward.fbx"]);
    avatar.motiono["crouchBackward"] = avatar.mixer->createMotion(animationo["Sneaking Forward reverse.fbx"]);
    avatar.motiono["crouchLeft"] = avatar.mixer->createMotion(animationo["Crouched Sneaking Left.fbx"]);
    avatar.motiono["crouchRight"] = avatar.mixer->createMotion(animationo["Crouched Sneaking Right.fbx"]);
    avatar.motiono["crouchLeftMirror"] = avatar.mixer->createMotion(animationo["Crouched Sneaking Right reverse.fbx"]);
    avatar.motiono["crouchRightMirror"] = avatar.mixer->createMotion(animationo["Crouched Sneaking Left reverse.fbx"]);

    avatar.motiono["bowForward"] = avatar.mixer->createMotion(animationo["Standing Aim Walk Forward.fbx"]);
    avatar.motiono["bowBackward"] = avatar.mixer->createMotion(animationo["Standing Aim Walk Forward reverse.fbx"]);
    avatar.motiono["bowLeft"] = avatar.mixer->createMotion(animationo["Standing Aim Walk Left.fbx"]);
    avatar.motiono["bowRight"] = avatar.mixer->createMotion(animationo["Standing Aim Walk Right.fbx"]);
    avatar.motiono["bowLeftMirror"] = avatar.mixer->createMotion(animationo["Standing Aim Walk Right reverse.fbx"]);
    avatar.motiono["bowRightMirror"] = avatar.mixer->createMotion(animationo["Standing Aim Walk Left reverse.fbx"]);

    avatar.motiono["crouchIdle"] = avatar.mixer->createMotion(animationo["Crouch Idle.fbx"]);
    avatar.motiono["fly"] = avatar.mixer->createMotion(animationo["treading water.fbx"]);
    avatar.motiono["flyIdle"] = avatar.mixer->createMotion(animationo["fly_idle.fbx"]);
    avatar.motiono["flyDodgeForward"] = avatar.mixer->createMotion(animationo["fly_dodge_forward.fbx"]);
    avatar.motiono["flyDodgeBackward"] = avatar.mixer->createMotion(animationo["fly_dodge_backward.fbx"]);
    avatar.motiono["flyDodgeLeft"] = avatar.mixer->createMotion(animationo["fly_dodge_left.fbx"]);
    avatar.motiono["flyDodgeRight"] = avatar.mixer->createMotion(animationo["fly_dodge_right.fbx"]);
    avatar.motiono["flyDash"] = avatar.mixer->createMotion(animationo["fly_dash_forward.fbx"]);
    avatar.motiono["narutoRun"] = avatar.mixer->createMotion(animationo["naruto run.fbx"]);

    avatar.motiono["jump"] = avatar.mixer->createMotion(animationo["jump.fbx"]);
    avatar.motiono["doubleJump"] = avatar.mixer->createMotion(animationo["jump_double.fbx"]);
    avatar.motiono["fallLoop"] = avatar.mixer->createMotion(animationo["falling.fbx"]);
    avatar.motiono["land"] = avatar.mixer->createMotion(animationo["landing.fbx"]);
    avatar.motiono["land2"] = avatar.mixer->createMotion(animationo["landing 2.fbx"]);

    // useAnimations
    avatar.motiono["combo"] = avatar.mixer->createMotion(animationo["One Hand Sword Combo.fbx"]);
    avatar.motiono["dashAttack"] = avatar.mixer->createMotion(animationo["sword_dash.fbx"]);
    avatar.motiono["drink"] = avatar.mixer->createMotion(animationo["drinking.fbx"]);
    avatar.motiono["eat"] = avatar.mixer->createMotion(animationo["eating.fbx"]);
    avatar.motiono["magic"] = avatar.mixer->createMotion(animationo["magic cast.fbx"]);
    avatar.motiono["pickUpThrow"] = avatar.mixer->createMotion(animationo["pick_up_throw.fbx"]);
    avatar.motiono["pistol"] = avatar.mixer->createMotion(animationo["Pistol Aiming Idle.fbx"]);
    avatar.motiono["rifle"] = avatar.mixer->createMotion(animationo["Rifle Aiming Idle.fbx"]);
    avatar.motiono["slash"] = avatar.mixer->createMotion(animationo["sword and shield slash.fbx"]);
    avatar.motiono["throw"] = avatar.mixer->createMotion(animationo["pick_up_throw.fbx"]);
    
    // useComboAnimations
    avatar.motiono["swordSideIdle"] = avatar.mixer->createMotion(animationo["sword_idle_side.fbx"]);
    avatar.motiono["swordSideSlash"] = avatar.mixer->createMotion(animationo["sword_side_slash.fbx"]);
    avatar.motiono["swordSideSlashStep"] = avatar.mixer->createMotion(animationo["sword_side_slash_step.fbx"]);
    avatar.motiono["swordTopDownSlash"] = avatar.mixer->createMotion(animationo["sword_topdown_slash.fbx"]);
    avatar.motiono["swordTopDownSlashStep"] = avatar.mixer->createMotion(animationo["sword_topdown_slash_step.fbx"]);
    avatar.motiono["dashAttack"] = avatar.mixer->createMotion(animationo["sword_dash.fbx"]);

    // bowAnimations
    avatar.motiono["bowDraw"] = avatar.mixer->createMotion(animationo["bow draw.fbx"]);
    avatar.motiono["bowIdle"] = avatar.mixer->createMotion(animationo["bow idle.fbx"]);
    avatar.motiono["bowLoose"] = avatar.mixer->createMotion(animationo["bow loose.fbx"]);

    // sitAnimations
    avatar.motiono["chair"] = avatar.mixer->createMotion(animationo["sitting idle.fbx"]);
    avatar.motiono["saddle"] = avatar.mixer->createMotion(animationo["sitting idle.fbx"]);
    avatar.motiono["stand"] = avatar.mixer->createMotion(animationo["Skateboarding.fbx"]);

    // hurtAnimations
    avatar.motiono["pain_back"] = avatar.mixer->createMotion(animationo["pain_back.fbx"]);
    avatar.motiono["pain_arch"] = avatar.mixer->createMotion(animationo["pain_arch.fbx"]);

    // emoteAnimations
    avatar.motiono["alert"] = avatar.mixer->createMotion(animationo["alert.fbx"]);
    avatar.motiono["alertSoft"] = avatar.mixer->createMotion(animationo["alert_soft.fbx"]);
    avatar.motiono["angry"] = avatar.mixer->createMotion(animationo["angry.fbx"]);
    avatar.motiono["angrySoft"] = avatar.mixer->createMotion(animationo["angry_soft.fbx"]);
    avatar.motiono["embarrassed"] = avatar.mixer->createMotion(animationo["embarrassed.fbx"]);
    avatar.motiono["embarrassedSoft"] = avatar.mixer->createMotion(animationo["embarrassed_soft.fbx"]);
    avatar.motiono["headNod"] = avatar.mixer->createMotion(animationo["head_nod.fbx"]);
    avatar.motiono["headNodSoft"] = avatar.mixer->createMotion(animationo["head_nod_single.fbx"]);
    avatar.motiono["headShake"] = avatar.mixer->createMotion(animationo["head_shake.fbx"]);
    avatar.motiono["headShakeSoft"] = avatar.mixer->createMotion(animationo["head_shake_single.fbx"]);
    avatar.motiono["sad"] = avatar.mixer->createMotion(animationo["sad.fbx"]);
    avatar.motiono["sadSoft"] = avatar.mixer->createMotion(animationo["sad_soft.fbx"]);
    avatar.motiono["surprise"] = avatar.mixer->createMotion(animationo["surprise.fbx"]);
    avatar.motiono["surpriseSoft"] = avatar.mixer->createMotion(animationo["surprise_soft.fbx"]);
    avatar.motiono["victory"] = avatar.mixer->createMotion(animationo["victory.fbx"]);
    avatar.motiono["victorySoft"] = avatar.mixer->createMotion(animationo["victory_soft.fbx"]);

    // danceAnimations
    avatar.motiono["dansu"] = avatar.mixer->createMotion(animationo["Hip Hop Dancing.fbx"]);
    avatar.motiono["powerup"] = avatar.mixer->createMotion(animationo["powerup.fbx"]);

    // holdAnimations
    avatar.motiono["pick_up_idle"] = avatar.mixer->createMotion(animationo["pick_up_idle.fbx"]);

    // activateAnimations
    avatar.motiono["grab_forward"] = avatar.mixer->createMotion(animationo["grab_forward.fbx"]);
    avatar.motiono["grab_down"] = avatar.mixer->createMotion(animationo["grab_down.fbx"]);
    avatar.motiono["grab_up"] = avatar.mixer->createMotion(animationo["grab_up.fbx"]);
    avatar.motiono["grab_left"] = avatar.mixer->createMotion(animationo["grab_left.fbx"]);
    avatar.motiono["grab_right"] = avatar.mixer->createMotion(animationo["grab_right.fbx"]);
    avatar.motiono["pick_up"] = avatar.mixer->createMotion(animationo["pick_up.fbx"]);

    //
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);
    // avatar.motiono[] = avatar.mixer->createMotion(animationo[]);

    // create node ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    avatar.nodeo["_8DirectionsWalkNodeList"] = avatar.mixer->createNode(NodeType::LIST);
    addChild(avatar.nodeo["_8DirectionsWalkNodeList"], avatar.motiono["walkForward"]);
    addChild(avatar.nodeo["_8DirectionsWalkNodeList"], avatar.motiono["walkBackward"]);
    addChild(avatar.nodeo["_8DirectionsWalkNodeList"], avatar.motiono["walkLeft"]);
    addChild(avatar.nodeo["_8DirectionsWalkNodeList"], avatar.motiono["walkRight"]);
    addChild(avatar.nodeo["_8DirectionsWalkNodeList"], avatar.motiono["walkLeftMirror"]);
    addChild(avatar.nodeo["_8DirectionsWalkNodeList"], avatar.motiono["walkRightMirror"]);

    avatar.nodeo["_8DirectionsRunNodeList"] = avatar.mixer->createNode(NodeType::LIST);
    addChild(avatar.nodeo["_8DirectionsRunNodeList"], avatar.motiono["runForward"]);
    addChild(avatar.nodeo["_8DirectionsRunNodeList"], avatar.motiono["runBackward"]);
    addChild(avatar.nodeo["_8DirectionsRunNodeList"], avatar.motiono["runLeft"]);
    addChild(avatar.nodeo["_8DirectionsRunNodeList"], avatar.motiono["runRight"]);
    addChild(avatar.nodeo["_8DirectionsRunNodeList"], avatar.motiono["runLeftMirror"]);
    addChild(avatar.nodeo["_8DirectionsRunNodeList"], avatar.motiono["runRightMirror"]);

    avatar.nodeo["_8DirectionsCrouchNodeList"] = avatar.mixer->createNode(NodeType::LIST);
    addChild(avatar.nodeo["_8DirectionsCrouchNodeList"], avatar.motiono["crouchForward"]);
    addChild(avatar.nodeo["_8DirectionsCrouchNodeList"], avatar.motiono["crouchBackward"]);
    addChild(avatar.nodeo["_8DirectionsCrouchNodeList"], avatar.motiono["crouchLeft"]);
    addChild(avatar.nodeo["_8DirectionsCrouchNodeList"], avatar.motiono["crouchRight"]);
    addChild(avatar.nodeo["_8DirectionsCrouchNodeList"], avatar.motiono["crouchLeftMirror"]);
    addChild(avatar.nodeo["_8DirectionsCrouchNodeList"], avatar.motiono["crouchRightMirror"]);

    avatar.nodeo["_8DirectionsBowNodeList"] = avatar.mixer->createNode(NodeType::LIST);
    addChild(avatar.nodeo["_8DirectionsBowNodeList"], avatar.motiono["bowForward"]);
    addChild(avatar.nodeo["_8DirectionsBowNodeList"], avatar.motiono["bowBackward"]);
    addChild(avatar.nodeo["_8DirectionsBowNodeList"], avatar.motiono["bowLeft"]);
    addChild(avatar.nodeo["_8DirectionsBowNodeList"], avatar.motiono["bowRight"]);
    addChild(avatar.nodeo["_8DirectionsBowNodeList"], avatar.motiono["bowLeftMirror"]);
    addChild(avatar.nodeo["_8DirectionsBowNodeList"], avatar.motiono["bowRightMirror"]);

    avatar.nodeo["_8DirectionsWalkRunNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["_8DirectionsWalkRunNodeTwo"], avatar.nodeo["_8DirectionsWalkNodeList"]);
    addChild(avatar.nodeo["_8DirectionsWalkRunNodeTwo"], avatar.nodeo["_8DirectionsRunNodeList"]);

    avatar.nodeo["idle8DWalkRunNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["idle8DWalkRunNodeTwo"], avatar.motiono["idle"]);
    addChild(avatar.nodeo["idle8DWalkRunNodeTwo"], avatar.nodeo["_8DirectionsWalkRunNodeTwo"]);

    avatar.nodeo["idle8DCrouchNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["idle8DCrouchNodeTwo"], avatar.motiono["crouchIdle"]);
    addChild(avatar.nodeo["idle8DCrouchNodeTwo"], avatar.nodeo["_8DirectionsCrouchNodeList"]);

    avatar.nodeo["flyForwardNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["flyForwardNodeTwo"], avatar.motiono["flyDodgeForward"]);
    addChild(avatar.nodeo["flyForwardNodeTwo"], avatar.motiono["flyDash"]);

    avatar.nodeo["_8DirectionsFlyNodeList"] = avatar.mixer->createNode(NodeType::LIST);
    addChild(avatar.nodeo["_8DirectionsFlyNodeList"], avatar.nodeo["flyForwardNodeTwo"]);
    addChild(avatar.nodeo["_8DirectionsFlyNodeList"], avatar.motiono["flyDodgeBackward"]);
    addChild(avatar.nodeo["_8DirectionsFlyNodeList"], avatar.motiono["flyDodgeLeft"]);
    addChild(avatar.nodeo["_8DirectionsFlyNodeList"], avatar.motiono["flyDodgeRight"]);

    avatar.nodeo["idle8DFlyNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["idle8DFlyNodeTwo"], avatar.motiono["flyIdle"]);
    addChild(avatar.nodeo["idle8DFlyNodeTwo"], avatar.nodeo["_8DirectionsFlyNodeList"]);

    avatar.nodeo["idle8DBowNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["idle8DBowNodeTwo"], avatar.motiono["bowIdle"]);
    addChild(avatar.nodeo["idle8DBowNodeTwo"], avatar.nodeo["_8DirectionsBowNodeList"]);

    avatar.nodeo["bowDrawLooseNodoeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["bowDrawLooseNodoeTwo"], avatar.motiono["bowDraw"]);
    addChild(avatar.nodeo["bowDrawLooseNodoeTwo"], avatar.motiono["bowLoose"]);

    avatar.nodeo["bowIdle8DDrawLooseNodeOverwrite"] = avatar.mixer->createNode(NodeType::OVERWRITE);
    addChild(avatar.nodeo["bowIdle8DDrawLooseNodeOverwrite"], avatar.nodeo["idle8DBowNodeTwo"]);
    addChild(avatar.nodeo["bowIdle8DDrawLooseNodeOverwrite"], avatar.nodeo["bowDrawLooseNodoeTwo"]);

    avatar.nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"], avatar.nodeo["idle8DWalkRunNodeTwo"]);
    addChild(avatar.nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"], avatar.nodeo["bowIdle8DDrawLooseNodeOverwrite"]);

    avatar.nodeo["defaultNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["defaultNodeTwo"], avatar.nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]);
    addChild(avatar.nodeo["defaultNodeTwo"], avatar.nodeo["idle8DCrouchNodeTwo"]);

    // hurtAnimations
    avatar.nodeo["hurtsNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    addChild(avatar.nodeo["hurtsNodeSolitary"], avatar.motiono["pain_back"]);
    addChild(avatar.nodeo["hurtsNodeSolitary"], avatar.motiono["pain_arch"]);
    //
    avatar.nodeo["hurtNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["hurtNodeTwo"], avatar.nodeo["defaultNodeTwo"]);
    addChild(avatar.nodeo["hurtNodeTwo"], avatar.nodeo["hurtsNodeSolitary"]);

    // useAnimations
    avatar.nodeo["usesNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["combo"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["dashAttack"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["drink"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["eat"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["magic"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["pickUpThrow"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["pistol"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["rifle"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["slash"]);
    addChild(avatar.nodeo["usesNodeSolitary"], avatar.motiono["throw"]);
    //
    avatar.nodeo["useNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["useNodeTwo"], avatar.nodeo["hurtNodeTwo"]);
    addChild(avatar.nodeo["useNodeTwo"], avatar.nodeo["usesNodeSolitary"]);

    // useComboAnimations
    avatar.nodeo["useCombosNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    //
    addChild(avatar.nodeo["useCombosNodeSolitary"], avatar.nodeo["useNodeTwo"]);
    //
    addChild(avatar.nodeo["useCombosNodeSolitary"], avatar.motiono["swordSideIdle"]);
    addChild(avatar.nodeo["useCombosNodeSolitary"], avatar.motiono["swordSideSlash"]);
    addChild(avatar.nodeo["useCombosNodeSolitary"], avatar.motiono["swordSideSlashStep"]);
    addChild(avatar.nodeo["useCombosNodeSolitary"], avatar.motiono["swordTopDownSlash"]);
    addChild(avatar.nodeo["useCombosNodeSolitary"], avatar.motiono["swordTopDownSlashStep"]);
    addChild(avatar.nodeo["useCombosNodeSolitary"], avatar.motiono["dashAttack"]);

    // emoteAnimations
    avatar.nodeo["emotesNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["alert"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["alertSoft"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["angry"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["angrySoft"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["embarrassed"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["embarrassedSoft"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["headNod"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["headNodSoft"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["headShake"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["headShakeSoft"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["sad"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["sadSoft"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["surprise"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["surpriseSoft"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["victory"]);
    addChild(avatar.nodeo["emotesNodeSolitary"], avatar.motiono["victorySoft"]);
    //
    avatar.nodeo["emoteNodeFunc"] = avatar.mixer->createNode(NodeType::FUNC, 1);
    addChild(avatar.nodeo["emoteNodeFunc"], avatar.nodeo["useCombosNodeSolitary"]);
    addChild(avatar.nodeo["emoteNodeFunc"], avatar.nodeo["emotesNodeSolitary"]);

    // danceAnimations
    avatar.nodeo["dancesNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    addChild(avatar.nodeo["dancesNodeSolitary"], avatar.motiono["dansu"]);
    addChild(avatar.nodeo["dancesNodeSolitary"], avatar.motiono["powerup"]);
    //
    avatar.nodeo["danceNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["danceNodeTwo"], avatar.nodeo["emoteNodeFunc"]);
    addChild(avatar.nodeo["danceNodeTwo"], avatar.nodeo["dancesNodeSolitary"]);

    avatar.nodeo["narutoRunNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["narutoRunNodeTwo"], avatar.nodeo["danceNodeTwo"]);
    addChild(avatar.nodeo["narutoRunNodeTwo"], avatar.motiono["narutoRun"]);

    // sitAnimations
    avatar.nodeo["sitsNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    addChild(avatar.nodeo["sitsNodeSolitary"], avatar.motiono["chair"]);
    addChild(avatar.nodeo["sitsNodeSolitary"], avatar.motiono["saddle"]);
    addChild(avatar.nodeo["sitsNodeSolitary"], avatar.motiono["stand"]);
    //
    avatar.nodeo["sitNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["sitNodeTwo"], avatar.nodeo["narutoRunNodeTwo"]);
    addChild(avatar.nodeo["sitNodeTwo"], avatar.nodeo["sitsNodeSolitary"]);

    avatar.nodeo["jumpNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["jumpNodeTwo"], avatar.nodeo["sitNodeTwo"]);
    addChild(avatar.nodeo["jumpNodeTwo"], avatar.motiono["jump"]);

    avatar.nodeo["doubleJumpNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["doubleJumpNodeTwo"], avatar.nodeo["jumpNodeTwo"]);
    addChild(avatar.nodeo["doubleJumpNodeTwo"], avatar.motiono["doubleJump"]);

    avatar.nodeo["groundFlyNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["groundFlyNodeTwo"], avatar.nodeo["doubleJumpNodeTwo"]);
    addChild(avatar.nodeo["groundFlyNodeTwo"], avatar.nodeo["idle8DFlyNodeTwo"]);

    avatar.nodeo["fallLoopNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["fallLoopNodeTwo"], avatar.nodeo["groundFlyNodeTwo"]);
    addChild(avatar.nodeo["fallLoopNodeTwo"], avatar.motiono["fallLoop"]);

    avatar.nodeo["landsNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    addChild(avatar.nodeo["landsNodeSolitary"], avatar.motiono["land"]);
    addChild(avatar.nodeo["landsNodeSolitary"], avatar.motiono["land2"]);
    //
    avatar.nodeo["landNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["landNodeTwo"], avatar.nodeo["fallLoopNodeTwo"]);
    addChild(avatar.nodeo["landNodeTwo"], avatar.nodeo["landsNodeSolitary"]);

    // activateAnimations
    avatar.nodeo["activatesNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    addChild(avatar.nodeo["activatesNodeSolitary"], avatar.motiono["grab_forward"]);
    addChild(avatar.nodeo["activatesNodeSolitary"], avatar.motiono["grab_down"]);
    addChild(avatar.nodeo["activatesNodeSolitary"], avatar.motiono["grab_up"]);
    addChild(avatar.nodeo["activatesNodeSolitary"], avatar.motiono["grab_left"]);
    addChild(avatar.nodeo["activatesNodeSolitary"], avatar.motiono["grab_right"]);
    addChild(avatar.nodeo["activatesNodeSolitary"], avatar.motiono["pick_up"]);
    //
    avatar.nodeo["activateNodeTwo"] = avatar.mixer->createNode(NodeType::TWO);
    addChild(avatar.nodeo["activateNodeTwo"], avatar.nodeo["landNodeTwo"]);
    addChild(avatar.nodeo["activateNodeTwo"], avatar.nodeo["activatesNodeSolitary"]);

    // holdAnimations
    avatar.nodeo["holdsNodeSolitary"] = avatar.mixer->createNode(NodeType::SOLITARY);
    addChild(avatar.nodeo["holdsNodeSolitary"], avatar.motiono["pick_up_idle"]);
    //
    avatar.nodeo["holdNodeFunc"] = avatar.mixer->createNode(NodeType::FUNC, 0);
    addChild(avatar.nodeo["holdNodeFunc"], avatar.nodeo["activateNodeTwo"]);
    addChild(avatar.nodeo["holdNodeFunc"], avatar.nodeo["holdsNodeSolitary"]);

    // set root node ---
    setRootNode(avatar.mixer, avatar.nodeo["holdNodeFunc"]);
  }
  void updateAvatar(float *scratchStack)
  {
    // std::cout << "idleWalkFactor: " << scratchStack[index] << std::endl;

    unsigned int index = 0;

    // values ---
    float forwardFactor = scratchStack[index++];
    float backwardFactor = scratchStack[index++];
    float leftFactor = scratchStack[index++];
    float rightFactor = scratchStack[index++];
    float mirrorLeftFactorReverse = scratchStack[index++];
    float mirrorLeftFactor = scratchStack[index++];
    float mirrorRightFactorReverse = scratchStack[index++];
    float mirrorRightFactor = scratchStack[index++];

    float idleWalkFactor = scratchStack[index++];
    float walkRunFactor = scratchStack[index++];
    float crouchFactor = scratchStack[index++];
    float flyDashFactor = scratchStack[index++];

    float holdFactor = scratchStack[index++];

    // action end events ---
    float landEnd = scratchStack[index++];
    float fallLoopEnd = scratchStack[index++];
    float flyEnd = scratchStack[index++];
    float jumpEnd = scratchStack[index++];
    float doubleJumpEnd = scratchStack[index++];
    float narutoRunEnd = scratchStack[index++];
    float activateEnd = scratchStack[index++];
    float useEnd = scratchStack[index++];
    float useComboEnd = scratchStack[index++];
    float useEnvelopeEnd = scratchStack[index++];
    float sitEnd = scratchStack[index++];
    float emoteEnd = scratchStack[index++];
    float hurtEnd = scratchStack[index++];
    float danceEnd = scratchStack[index++];
    float holdEnd = scratchStack[index++];

    // action start events ---
    float landStart = scratchStack[index++];
    float fallLoopStart = scratchStack[index++];
    float flyStart = scratchStack[index++];
    float jumpStart = scratchStack[index++];
    float doubleJumpStart = scratchStack[index++];
    float narutoRunStart = scratchStack[index++];
    float useStart = scratchStack[index++];
    float useComboStart = scratchStack[index++];
    float useEnvelopeStart = scratchStack[index++];
    float sitStart = scratchStack[index++];
    float emoteStart = scratchStack[index++];
    float hurtStart = scratchStack[index++];
    float danceStart = scratchStack[index++];
    float holdStart = scratchStack[index++];
    float activateStart = scratchStack[index++];

    // other ---
    float landWithMoving = scratchStack[index++];
    float dashAttacking = scratchStack[index++];

    // values ---
    setWeight(avatar.motiono["walkForward"], forwardFactor);
    setWeight(avatar.motiono["walkBackward"], backwardFactor);
    setWeight(avatar.motiono["walkLeft"], mirrorLeftFactorReverse);
    setWeight(avatar.motiono["walkLeftMirror"], mirrorLeftFactor);
    setWeight(avatar.motiono["walkRight"], mirrorRightFactorReverse);
    setWeight(avatar.motiono["walkRightMirror"], mirrorRightFactor);

    setWeight(avatar.motiono["runForward"], forwardFactor);
    setWeight(avatar.motiono["runBackward"], backwardFactor);
    setWeight(avatar.motiono["runLeft"], mirrorLeftFactorReverse);
    setWeight(avatar.motiono["runLeftMirror"], mirrorLeftFactor);
    setWeight(avatar.motiono["runRight"], mirrorRightFactorReverse);
    setWeight(avatar.motiono["runRightMirror"], mirrorRightFactor);

    setWeight(avatar.motiono["crouchForward"], forwardFactor);
    setWeight(avatar.motiono["crouchBackward"], backwardFactor);
    setWeight(avatar.motiono["crouchLeft"], mirrorLeftFactorReverse);
    setWeight(avatar.motiono["crouchLeftMirror"], mirrorLeftFactor);
    setWeight(avatar.motiono["crouchRight"], mirrorRightFactorReverse);
    setWeight(avatar.motiono["crouchRightMirror"], mirrorRightFactor);

    setWeight(avatar.motiono["bowForward"], forwardFactor);
    setWeight(avatar.motiono["bowBackward"], backwardFactor);
    setWeight(avatar.motiono["bowLeft"], mirrorLeftFactorReverse);
    setWeight(avatar.motiono["bowLeftMirror"], mirrorLeftFactor);
    setWeight(avatar.motiono["bowRight"], mirrorRightFactorReverse);
    setWeight(avatar.motiono["bowRightMirror"], mirrorRightFactor);

    setFactor(avatar.nodeo["_8DirectionsWalkRunNodeTwo"], walkRunFactor);
    setFactor(avatar.nodeo["idle8DWalkRunNodeTwo"], idleWalkFactor);
    setFactor(avatar.nodeo["idle8DCrouchNodeTwo"], idleWalkFactor);
    setFactor(avatar.nodeo["defaultNodeTwo"], crouchFactor);
    setFactor(avatar.nodeo["idle8DBowNodeTwo"], idleWalkFactor);

    setWeight(avatar.nodeo["flyForwardNodeTwo"], forwardFactor);
    setWeight(avatar.motiono["flyDodgeBackward"], backwardFactor);
    setWeight(avatar.motiono["flyDodgeLeft"], leftFactor);
    setWeight(avatar.motiono["flyDodgeRight"], rightFactor);

    setFactor(avatar.nodeo["idle8DFlyNodeTwo"], walkRunFactor);
    setFactor(avatar.nodeo["flyForwardNodeTwo"], flyDashFactor);

    setArg(avatar.nodeo["holdNodeFunc"], holdFactor);
    setArg(avatar.nodeo["emoteNodeFunc"], idleWalkFactor);

    // action end events ---
    if (landEnd) {
      if (!landWithMoving) {
        avatar.nodeo["landNodeTwo"]->crossFadeTwo(0.05, 0);
      } else {
        avatar.nodeo["landNodeTwo"]->crossFadeTwo(0.15, 0);
      }
    }
    
    if (fallLoopEnd) {
      avatar.nodeo["fallLoopNodeTwo"]->crossFadeTwo(0.2, 0);
    }
    
    if (flyEnd) {
      avatar.nodeo["groundFlyNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (jumpEnd) {
      avatar.nodeo["jumpNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (doubleJumpEnd) {
      avatar.nodeo["doubleJumpNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (narutoRunEnd) {
      avatar.nodeo["narutoRunNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (activateEnd) {
      avatar.nodeo["activateNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (useEnd) {
      avatar.nodeo["useNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (useComboEnd) {
      avatar.nodeo["useCombosNodeSolitary"]->crossFadeSolitary(0.2, avatar.nodeo["useNodeTwo"]);
    }

    if (useEnvelopeEnd) {
      avatar.motiono["bowLoose"]->play();
      setFactor(avatar.nodeo["bowDrawLooseNodoeTwo"], 1);
      avatar.nodeo["bowIdle8DDrawLooseNodeOverwrite"]->crossFadeTwo(0.2, 1);
    }

    if (sitEnd) {
      avatar.nodeo["sitNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (emoteEnd) {
      avatar.nodeo["emoteNodeFunc"]->crossFadeTwo(0.2, 0);
    }

    if (hurtEnd) {
      avatar.nodeo["hurtNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (danceEnd) {
      avatar.nodeo["danceNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (holdEnd) {
      avatar.nodeo["holdNodeFunc"]->crossFadeTwo(0.2, 0);
    }

    // action start events ---
    if (landStart) {
      std::cout << landWithMoving << std::endl;
      std::cout << (bool)(landWithMoving) << std::endl;
      if (!landWithMoving) {
        avatar.motiono["land"]->play();
        avatar.nodeo["landsNodeSolitary"]->crossFadeSolitary(0, avatar.motiono["land"]);
        avatar.nodeo["landNodeTwo"]->crossFadeTwo(0, 1);
      } else {
        avatar.motiono["land2"]->play();
        avatar.nodeo["landsNodeSolitary"]->crossFadeSolitary(0, avatar.motiono["land2"]);
        avatar.nodeo["landNodeTwo"]->crossFadeTwo(0.1, 1);
      }
    }

    if (fallLoopStart) {
      avatar.nodeo["fallLoopNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (flyStart) {
      avatar.nodeo["groundFlyNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (jumpStart) {
      avatar.motiono["jumpMotion"]->play();
      avatar.nodeo["jumpNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (doubleJumpStart) {
      avatar.motiono["doubleJumpMotion"]->play();
      avatar.nodeo["doubleJumpNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (narutoRunStart) {
      avatar.nodeo["narutoRunNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // // sword
    // if (useStart) {
    //   let useAnimationName;
    //   if (dashAttacking) {
    //     useAnimationName = 'dashAttack';
    //   } else {
    //     useAnimationName = avatar.useAnimation;
    //   }
    //   const useMotion = avatar.useMotionPtro[useAnimationName];
    //   physx.physxWorker.play(useMotion);
    //   physx.physxWorker.crossFadeSolitary(avatar.usesNodeSolitaryPtr, 0, useMotion);
    //   avatar.nodeo["useNodeTwo"].crossFadeTwo(0.2, 1);
    // }

    // // silsword
    // if (useComboStart) {
    //   let useAnimationName;
    //   if (dashAttacking) {
    //     useAnimationName = 'dashAttack';
    //   } else {
    //     useAnimationName = avatar.useAnimationCombo[avatar.useAnimationIndex];
    //   }
    //   const useMotion = avatar.useComboMotionPtro[useAnimationName];
    //   physx.physxWorker.play(useMotion);
    //   physx.physxWorker.crossFadeSolitary(avatar.useCombosNodeSolitaryPtr, 0.2, useMotion);
    // }

    // // bow
    // if (useEnvelopeStart) {
    //   console.log('useEnvelopeStart');
    //   physx.physxWorker.play(avatar.bowMotionPtro.bowDraw);
    //   physx.physxWorker.setFactor(avatar.bowDrawLooseNodoeTwoPtr, 0);
    //   physx.physxWorker.setFactor(avatar.bowIdle8DDrawLooseNodeOverwritePtr, 1);
    //   avatar.nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"].crossFadeTwo(0.2, 1);
    // }

    // // sit
    // if (sitStart) {
    //   const sitMotion = avatar.sitMotionPtro[avatar.sitAnimation || defaultSitAnimation];
    //   physx.physxWorker.play(sitMotion);
    //   physx.physxWorker.crossFadeSolitary(avatar.sitsNodeSolitaryPtr, 0, sitMotion);
    //   avatar.nodeo["sitNodeTwo"].crossFadeTwo(0.2, 1);
    // }

    // // emote
    // if (emoteStart) {
    //   const emoteMotion = avatar.emoteMotionPtro[avatar.emoteAnimation || defaultEmoteAnimation];
    //   physx.physxWorker.play(emoteMotion);
    //   physx.physxWorker.crossFadeSolitary(avatar.emotesNodeSolitaryPtr, 0, emoteMotion);
    //   avatar.nodeo["emoteNodeFunc"].crossFadeTwo(0.2, 1);
    //   // physx.physxWorker.setFactor(avatar.emoteNodeFuncPtr, 1);
    // }

    // // hurt
    // if (hurtStart) {
    //   const hurtMotion = avatar.hurtMotionPtro[avatar.hurtAnimation];
    //   physx.physxWorker.play(hurtMotion);
    //   physx.physxWorker.crossFadeSolitary(avatar.hurtsNodeSolitaryPtr, 0, hurtMotion);
    //   avatar.nodeo["hurtNodeTwo"].crossFadeTwo(0.2, 1);
    // }

    // // dance
    // if (danceStart) {
    //   const danceMotion = avatar.danceMotionPtro[avatar.danceAnimation || defaultDanceAnimation];
    //   physx.physxWorker.play(danceMotion);
    //   physx.physxWorker.crossFadeSolitary(avatar.dancesNodeSolitaryPtr, 0, danceMotion);
    //   avatar.nodeo["danceNodeTwo"].crossFadeTwo(0.2, 1);
    // }

    // // hold
    // if (holdStart) {
    //   const holdMotion = avatar.holdMotionPtro[avatar.holdAnimation || defaultHoldAnimation];
    //   physx.physxWorker.play(holdMotion);
    //   physx.physxWorker.crossFadeSolitary(avatar.holdsNodeSolitaryPtr, 0, holdMotion);
    //   avatar.nodeo["holdNodeFunc"].crossFadeTwo(0.2, 1);
    // }

    // // activate
    // if (activateStart) {
    //   const activateMotion = avatar.activateMotionPtro[avatar.activateAnimation || defaultActivateAnimation];
    //   physx.physxWorker.play(activateMotion);
    //   physx.physxWorker.crossFadeSolitary(avatar.activatesNodeSolitaryPtr, 0, activateMotion);
    //   avatar.nodeo["activateNodeTwo"].crossFadeTwo(0.2, 1);
    // }
  }
  AnimationMixer *createAnimationMixer()
  {
    AnimationMixer *animationMixer = new AnimationMixer();
    // animationMixer->index = _animationMixers.size();
    _animationMixers.push_back(animationMixer);
    // _finishedFlags[animationMixer->index] = 0;
    // return &animationMixer; // todo: warning: address of stack memory associated with local variable 'animationMixer' returned [-Wreturn-stack-address]
    return animationMixer;

    // // init old
    // AnimationNode *walkFlyNode = new AnimationNode();
    // walkFlyNode->children.push_back(_motions[96]); // 96 walk
    // walkFlyNode->children.push_back(_motions[92]); // 92 fly

    // AnimationNode *crouchNode = new AnimationNode();
    // crouchNode->children.push_back(walkFlyNode);
    // crouchNode->children.push_back(_motions[9]); // 9 Crouch Idle.fbx

    // rootNode = crouchNode;

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

    // setRootNode(crouchNode);
  }
  void addChild(AnimationNode *parent, AnimationNode *child)
  {
    parent->children.push_back(child);

    if (parent->type == NodeType::SOLITARY)
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
  void setRootNode(AnimationMixer *mixer, AnimationNode *node)
  {
    mixer->rootNode = node;
  }
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone, bool isTop, bool isArm)
  {
    AnimationMapping animationMapping;
    animationMapping.isPosition = isPosition;
    animationMapping.index = index;
    animationMapping.isFirstBone = isFirstBone;
    animationMapping.isLastBone = isLastBone;
    animationMapping.isTop = isTop;
    animationMapping.isArm = isArm;
    _animationMappings.push_back(animationMapping);
    // std::cout << "_animationMappings size: " << _animationMappings.size() << std::endl;
  }
  Animation *createAnimation(char *scratchStack, unsigned int nameByteLength, float duration)
  {
    Animation *animation = new Animation();
    animation->duration = duration;
    // std::cout << "nameByteLength: " << nameByteLength << std::endl;
    // for (unsigned int i = 0; i < nameByteLength; i++)
    // {
    //   std::cout << scratchStack[i] << std::endl;
    // }
    // https://www.geeksforgeeks.org/convert-character-array-to-string-in-c/
    std::string name = "";
    for (unsigned int i = 0; i < nameByteLength; i++)
    {
      name += scratchStack[i];
    }
    // std::cout << name << std::endl;

    animationo[name] = animation;

    return animation;
  }
  Animation *getAnimation(char *scratchStack, unsigned int nameByteLength)
  {
    std::string name = "";
    for (unsigned int i = 0; i < nameByteLength; i++)
    {
      name += scratchStack[i];
    }

    return animationo[name];
  }
  AnimationNode *AnimationMixer::createNode(NodeType type, unsigned int index)
  {
    AnimationNode *node = new AnimationNode();
    node->mixer = this;
    node->type = type;
    node->index = index;
    // std::cout << "NodeType: " << type << " " << node->type << std::endl;
    return node;
  }
  AnimationNode *AnimationMixer::createMotion(Animation *animation)
  {
    AnimationNode *motion = new AnimationNode();
    motion->mixer = this;
    motion->animation = animation;
    this->motions.push_back(motion);

    return motion;
  }
  AnimationNode *AnimationMixer::getMotion(char *scratchStack, unsigned int nameByteLength)
  {
    std::string name = "";
    for (unsigned int i = 0; i < nameByteLength; i++)
    {
      name += scratchStack[i];
    }

    return avatar.motiono[name];
  }
  AnimationNode *AnimationMixer::getNode(char *scratchStack, unsigned int nameByteLength)
  {
    std::string name = "";
    for (unsigned int i = 0; i < nameByteLength; i++)
    {
      name += scratchStack[i];
    }

    return avatar.nodeo[name];
  }
  void createInterpolant(char *scratchStack, unsigned int animationNameByteLength, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize)
  {

    // std::cout << "createInterpolant: " << numParameterPositions << " " << numSampleValues << " " << valueSize << std::endl;

    Interpolant interpolant;
    interpolant.numParameterPositions = numParameterPositions;
    interpolant.parameterPositions = parameterPositions;
    interpolant.resultBuffer = (float *)malloc((1 + valueSize) * sizeof(float)); // 1 len for valueSize self ( 3 or 4 ). 3 len or 4 len for values.
    interpolant.numSampleValues = numSampleValues;
    interpolant.sampleValues = sampleValues;
    interpolant.valueSize = valueSize; // only support 3 (vector) or 4 (quaternion)

    std::string name = "";
    for (unsigned int i = 0; i < animationNameByteLength; i++)
    {
      name += scratchStack[i];
    }
    animationo[name]->interpolants.push_back(interpolant);

    // std::cout
    // << "interpolant "
    // << interpolant.numParameterPositions
    // << interpolant.numSampleValues
    // << interpolant.valueSize
    // << _interpolants.size()
    // << std::endl;

    // std::cout << "interpolants size: " << _interpolants.size() << std::endl;
  }
  float *evaluateInterpolant(Animation *animation, unsigned int interpolantIndex, float t)
  {
    // std::cout << "evaluateInterpolant: " << interpolantIndex << " " << t << std::endl;

    // return _sampleValues[(int)t] + _parameterPositions[(int)t] + _valueSize;

    Interpolant interpolant = animation->interpolants[interpolantIndex];

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
  // float **getAnimationValues(unsigned int animationIndex, float t)
  // {
  //   for (int i = 0; i < 53; i++)
  //   {
  //     AnimationMapping spec = _animationMappings[i];
  //     animationValues[i] = evaluateInterpolant(animationIndex, i, t);
  //   }
  //   return animationValues;
  // }
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

  float **AnimationMixer::update(float timeS)
  {
    AnimationMixer::timeS = timeS;

    // reset
    // reset animation finished event.
    // finishedFlag = 0; // reset animation finished event.
    this->finishedFlag = 0;
    // *(animationValues[53]) = (float)0;
    // _finishedFlags[this->index] = 0;

    // return getAnimationValues(_animation.index, timeS); // Move `getAnimationValues()` to class AnimationMixer.

    for (int i = 0; i < 53; i++)
    {
      AnimationMapping spec = _animationMappings[i];

      // float * aaa = rootNode.update(spec);
      animationValues[i] = rootNode->update(spec);
    }

    // animationValues[53] = &finishedFlag;
    // animationValues[53] = &_finishedFlags[this->index];
    // animationValues[53] = &this->finishedFlag;
    // animationValues[54] = &finishedAnimationIndex;

    return animationValues;
  }

  float *AnimationNode::update(AnimationMapping &spec) // todo: &spec
  {
    // this->results[spec.index] = NULL; // todo: don't need ?
    for (unsigned int i = 0; i < this->children.size(); i++)
    {
      AnimationNode *childNode = this->children[i];
      childNode->results[spec.index] = NULL;
    }

    if (this->animation) // isMotion ------
    {
      float *value;
      float evaluateTimeS;
      // std::cout << "loop: " << this->loop << std::endl;
      if (this->loop == LoopType::LoopOnce)
      {
        evaluateTimeS = (AnimationMixer::timeS - this->startTime) * this->speed + this->timeBias;
        value = evaluateInterpolant(this->animation, spec.index, evaluateTimeS);
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
        if (/* spec.isLastBone &&  */ !this->isFinished && evaluateTimeS >= this->animation->duration) // Don't need and will cause bug if check `isLastBone`.
        {
          // std::cout << "finished: animation: index: " << this->animation->index << " pointer: " << this->animation << std::endl;
          // finishedFlag = 1;
          // _finishedFlags[this->mixer->index] = 1;
          this->mixer->finishedFlag = 1;
          // *(this->mixer->animationValues[53]) = (float)1; // todo: Must explicitly convert `1` (int) to float, otherwise will cause wrong value ?
          // finishedAnimationIndex = (float)this->animation->index; // Must explicitly convert index (unsigned int) to float, otherwise will cause wrong value.
          // animationValues[54] = this->mixer->motions
          for (int i = 0; i < this->mixer->motions.size(); i++)
          {
            AnimationNode *motion = this->mixer->motions[i];
            if (motion->animation == animation)
            {
              // std::cout << "finished: motion: pointer: " << motion << " pointer float: " << (float *)motion << std::endl;
              this->mixer->animationValues[54] = (float *)motion; // finished motion pointer
              break;
            }
          }

          this->isFinished = true;
        }
      }
      else
      {
        evaluateTimeS = fmod((AnimationMixer::timeS - this->startTime) * this->speed + this->timeBias, this->animation->duration);
        value = evaluateInterpolant(this->animation, spec.index, evaluateTimeS);
      }
      this->results[spec.index] = value;
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
      else if (this->type == NodeType::SOLITARY)
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
            {                                                            // ensure solitary
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
      else if (this->type == NodeType::OVERWRITE)
      {
        if (spec.isTop)
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
        else
        {
          this->children[0]->weight = 1;
          this->children[1]->weight = 0;
        }
      }
      else if (this->type == NodeType::FUNC)
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
        
        float *value0 = children[0]->update(spec);
        if (this->factor > 0)
        {
          float *value1 = children[1]->update(spec);

          if (this->index == 0) // hold animation
          {
            if (spec.isTop)
            {
              if (spec.index == BoneName::Left_arm || spec.index == BoneName::Right_arm)
              {
                // result = value1;
              }
              else
              {
                localQuaternion[1] = value0[1];
                localQuaternion[2] = value0[2];
                localQuaternion[3] = value0[3];
                localQuaternion[4] = value0[4];
                if (spec.isArm)
                {
                  interpolateFlat(localQuaternion, 1, localQuaternion, 1, identityQuaternion, 0, this->arg, spec.isPosition);
                }

                Quat quat0(localQuaternion[1], localQuaternion[2], localQuaternion[3], localQuaternion[4]);
                Quat quat1(value1[1], value1[2], value1[3], value1[4]);
                quat0.premultiply(quat1);
                value1[1] = quat0.x;
                value1[2] = quat0.y;
                value1[3] = quat0.z;
                value1[4] = quat0.w;
              }
            }
            else
            {
              value1[1] = value0[1];
              value1[2] = value0[2];
              value1[3] = value0[3];
              value1[4] = value0[4];
            }
          }
          else if (this->index == 1) // emote animation
          {
            if (spec.index == BoneName::Spine || spec.index == BoneName::Chest || spec.index == BoneName::UpperChest || spec.index == BoneName::Neck || spec.index == BoneName::Head) {
              if (!spec.isPosition) {
                Quat quat0(value0[1], value0[2], value0[3], value0[4]);
                Quat quat1(value1[1], value1[2], value1[3], value1[4]);
                quat0.premultiply(quat1);
                value1[1] = quat0.x;
                value1[2] = quat0.y;
                value1[3] = quat0.z;
                value1[4] = quat0.w;
              } else {
                interpolateFlat(value1, 1, value0, 1, value1, 1, this->factor, spec.isPosition);
              }
            } else {
              float f = this->factor;
              if (!spec.isTop) {
                f *= (1 - this->arg); // arg: idleWalkFactor
              }

              interpolateFlat(value1, 1, value0, 1, value1, 1, f, spec.isPosition);
            }
          }
        }

        this->children[0]->weight = 1 - this->factor;
        this->children[1]->weight = this->factor;
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
          float *value;
          if (childNode->results[spec.index])
          {
            value = childNode->results[spec.index];
            childNode->results[spec.index] = NULL;
          }
          else
          {
            value = childNode->update(spec);
          }
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
      this->results[spec.index] = result;
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
  void AnimationNode::crossFadeSolitary(float duration, AnimationNode *targetNode)
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