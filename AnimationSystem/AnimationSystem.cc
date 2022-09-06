#include "AnimationSystem.h"

namespace AnimationSystem
{
  // std::map<std::string, Interpolant *> interpolants;
  // std::vector<Interpolant> _interpolants;
  std::vector<Avatar *> avatars;
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

  Avatar *initAvatar(AnimationMixer *mixer) {
    // avatar->mixer = createAnimationMixer();
    Avatar *avatar = new Avatar();
    avatars.push_back(avatar);
    avatar->mixer = mixer;

    // create motion ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    avatar->motiono["idle"] = avatar->mixer->createMotion(animationo["idle.fbx"], "idle"); // todo: don't need `avatar->`

    avatar->motiono["walkForward"] = avatar->mixer->createMotion(animationo["walking.fbx"], "walkForward");
    avatar->motiono["walkBackward"] = avatar->mixer->createMotion(animationo["walking backwards.fbx"], "walkBackward");
    avatar->motiono["walkLeft"] = avatar->mixer->createMotion(animationo["left strafe walking.fbx"], "walkLeft");
    avatar->motiono["walkRight"] = avatar->mixer->createMotion(animationo["right strafe walking.fbx"], "walkRight");
    avatar->motiono["walkLeftMirror"] = avatar->mixer->createMotion(animationo["right strafe walking reverse.fbx"], "walkLeftMirror");
    avatar->motiono["walkRightMirror"] = avatar->mixer->createMotion(animationo["left strafe walking reverse.fbx"], "walkRightMirror");

    avatar->motiono["runForward"] = avatar->mixer->createMotion(animationo["Fast Run.fbx"], "runForward");
    avatar->motiono["runBackward"] = avatar->mixer->createMotion(animationo["running backwards.fbx"], "runBackward");
    avatar->motiono["runLeft"] = avatar->mixer->createMotion(animationo["left strafe.fbx"], "runLeft");
    avatar->motiono["runRight"] = avatar->mixer->createMotion(animationo["right strafe.fbx"], "runRight");
    avatar->motiono["runLeftMirror"] = avatar->mixer->createMotion(animationo["right strafe reverse.fbx"], "runLeftMirror");
    avatar->motiono["runRightMirror"] = avatar->mixer->createMotion(animationo["left strafe reverse.fbx"], "runRightMirror");

    avatar->motiono["crouchForward"] = avatar->mixer->createMotion(animationo["Sneaking Forward.fbx"], "crouchForward");
    avatar->motiono["crouchBackward"] = avatar->mixer->createMotion(animationo["Sneaking Forward reverse.fbx"], "crouchBackward");
    avatar->motiono["crouchLeft"] = avatar->mixer->createMotion(animationo["Crouched Sneaking Left.fbx"], "crouchLeft");
    avatar->motiono["crouchRight"] = avatar->mixer->createMotion(animationo["Crouched Sneaking Right.fbx"], "crouchRight");
    avatar->motiono["crouchLeftMirror"] = avatar->mixer->createMotion(animationo["Crouched Sneaking Right reverse.fbx"], "crouchLeftMirror");
    avatar->motiono["crouchRightMirror"] = avatar->mixer->createMotion(animationo["Crouched Sneaking Left reverse.fbx"], "crouchRightMirror");

    avatar->motiono["bowForward"] = avatar->mixer->createMotion(animationo["Standing Aim Walk Forward.fbx"], "bowForward");
    avatar->motiono["bowBackward"] = avatar->mixer->createMotion(animationo["Standing Aim Walk Forward reverse.fbx"], "bowBackward");
    avatar->motiono["bowLeft"] = avatar->mixer->createMotion(animationo["Standing Aim Walk Left.fbx"], "bowLeft");
    avatar->motiono["bowRight"] = avatar->mixer->createMotion(animationo["Standing Aim Walk Right.fbx"], "bowRight");
    avatar->motiono["bowLeftMirror"] = avatar->mixer->createMotion(animationo["Standing Aim Walk Right reverse.fbx"], "bowLeftMirror");
    avatar->motiono["bowRightMirror"] = avatar->mixer->createMotion(animationo["Standing Aim Walk Left reverse.fbx"], "bowRightMirror");

    avatar->motiono["crouchIdle"] = avatar->mixer->createMotion(animationo["Crouch Idle.fbx"], "crouchIdle");
    avatar->motiono["fly"] = avatar->mixer->createMotion(animationo["treading water.fbx"], "fly");
    avatar->motiono["flyIdle"] = avatar->mixer->createMotion(animationo["fly_idle.fbx"], "flyIdle");
    avatar->motiono["flyDodgeForward"] = avatar->mixer->createMotion(animationo["fly_dodge_forward.fbx"], "flyDodgeForward");
    avatar->motiono["flyDodgeBackward"] = avatar->mixer->createMotion(animationo["fly_dodge_backward.fbx"], "flyDodgeBackward");
    avatar->motiono["flyDodgeLeft"] = avatar->mixer->createMotion(animationo["fly_dodge_left.fbx"], "flyDodgeLeft");
    avatar->motiono["flyDodgeRight"] = avatar->mixer->createMotion(animationo["fly_dodge_right.fbx"], "flyDodgeRight");
    avatar->motiono["flyDash"] = avatar->mixer->createMotion(animationo["fly_dash_forward.fbx"], "flyDash");
    avatar->motiono["narutoRun"] = avatar->mixer->createMotion(animationo["naruto run.fbx"], "narutoRun");

    avatar->motiono["jump"] = avatar->mixer->createMotion(animationo["jump.fbx"], "jump");
    avatar->motiono["jump"]->setLoop(LoopType::LoopOnce);
    avatar->motiono["jump"]->stop();

    avatar->motiono["doubleJump"] = avatar->mixer->createMotion(animationo["jump_double.fbx"], "doubleJump");
    avatar->motiono["doubleJump"]->setLoop(LoopType::LoopOnce);
    avatar->motiono["doubleJump"]->stop();

    avatar->motiono["fallLoop"] = avatar->mixer->createMotion(animationo["falling.fbx"], "fallLoop");

    avatar->motiono["land"] = avatar->mixer->createMotion(animationo["landing.fbx"], "land");
    avatar->motiono["land"]->setLoop(LoopType::LoopOnce);
    avatar->motiono["land"]->stop();
    avatar->motiono["land"]->setSpeed(0.75);

    avatar->motiono["land2"] = avatar->mixer->createMotion(animationo["landing 2.fbx"], "land2");
    avatar->motiono["land2"]->setLoop(LoopType::LoopOnce);
    avatar->motiono["land2"]->stop();
    avatar->motiono["land2"]->setSpeed(1.7);

    // useAnimations
    avatar->useMotiono["combo"] = avatar->mixer->createMotion(animationo["One Hand Sword Combo.fbx"], "combo");
    avatar->useMotiono["dashAttack"] = avatar->mixer->createMotion(animationo["sword_dash.fbx"], "dashAttack");
    avatar->useMotiono["drink"] = avatar->mixer->createMotion(animationo["drinking.fbx"], "drink");
    avatar->useMotiono["eat"] = avatar->mixer->createMotion(animationo["eating.fbx"], "eat");
    avatar->useMotiono["magic"] = avatar->mixer->createMotion(animationo["magic cast.fbx"], "magic");
    avatar->useMotiono["pickUpThrow"] = avatar->mixer->createMotion(animationo["pick_up_throw.fbx"], "pickUpThrow");
    avatar->useMotiono["pistol"] = avatar->mixer->createMotion(animationo["Pistol Aiming Idle.fbx"], "pistol");
    avatar->useMotiono["rifle"] = avatar->mixer->createMotion(animationo["Rifle Aiming Idle.fbx"], "rifle");
    avatar->useMotiono["slash"] = avatar->mixer->createMotion(animationo["sword and shield slash.fbx"], "slash");
    avatar->useMotiono["throw"] = avatar->mixer->createMotion(animationo["pick_up_throw.fbx"], "throw");
    for (auto const& x : avatar->useMotiono)
    {
      avatar->useMotiono[x.first]->setLoop(LoopType::LoopOnce);
      avatar->useMotiono[x.first]->stop();
    }
    avatar->useMotiono["combo"]->setSpeed(1.3);
    
    // useComboAnimations
    avatar->useComboMotiono["swordSideIdle"] = avatar->mixer->createMotion(animationo["sword_idle_side.fbx"], "swordSideIdle");
    avatar->useComboMotiono["swordSideSlash"] = avatar->mixer->createMotion(animationo["sword_side_slash.fbx"], "swordSideSlash");
    avatar->useComboMotiono["swordSideSlashStep"] = avatar->mixer->createMotion(animationo["sword_side_slash_step.fbx"], "swordSideSlashStep");
    avatar->useComboMotiono["swordTopDownSlash"] = avatar->mixer->createMotion(animationo["sword_topdown_slash.fbx"], "swordTopDownSlash");
    avatar->useComboMotiono["swordTopDownSlashStep"] = avatar->mixer->createMotion(animationo["sword_topdown_slash_step.fbx"], "swordTopDownSlashStep");
    avatar->useComboMotiono["dashAttack"] = avatar->mixer->createMotion(animationo["sword_dash.fbx"], "dashAttack");
    for (auto const& x : avatar->useComboMotiono)
    {
      avatar->useComboMotiono[x.first]->setLoop(LoopType::LoopOnce);
      avatar->useComboMotiono[x.first]->stop();
    }

    // bowAnimations
    avatar->bowMotiono["bowDraw"] = avatar->mixer->createMotion(animationo["bow draw.fbx"], "bowDraw");
    avatar->bowMotiono["bowIdle"] = avatar->mixer->createMotion(animationo["bow idle.fbx"], "bowIdle");
    avatar->bowMotiono["bowLoose"] = avatar->mixer->createMotion(animationo["bow loose.fbx"], "bowLoose");
    for (auto const& x : avatar->bowMotiono)
    {
      avatar->bowMotiono[x.first]->setLoop(LoopType::LoopOnce);
      avatar->bowMotiono[x.first]->stop();
    }

    // sitAnimations
    avatar->sitMotiono["chair"] = avatar->mixer->createMotion(animationo["sitting idle.fbx"], "chair");
    avatar->sitMotiono["saddle"] = avatar->mixer->createMotion(animationo["sitting idle.fbx"], "saddle");
    avatar->sitMotiono["stand"] = avatar->mixer->createMotion(animationo["Skateboarding.fbx"], "stand");
    for (auto const& x : avatar->sitMotiono)
    {
      avatar->sitMotiono[x.first]->setLoop(LoopType::LoopOnce);
      avatar->sitMotiono[x.first]->stop();
    }

    // hurtAnimations
    avatar->hurtMotiono["pain_back"] = avatar->mixer->createMotion(animationo["pain_back.fbx"], "pain_back");
    avatar->hurtMotiono["pain_arch"] = avatar->mixer->createMotion(animationo["pain_arch.fbx"], "pain_arch");
    for (auto const& x : avatar->hurtMotiono)
    {
      avatar->hurtMotiono[x.first]->setLoop(LoopType::LoopOnce);
      avatar->hurtMotiono[x.first]->stop();
    }

    // emoteAnimations
    avatar->emoteMotiono["alert"] = avatar->mixer->createMotion(animationo["alert.fbx"], "alert");
    avatar->emoteMotiono["alertSoft"] = avatar->mixer->createMotion(animationo["alert_soft.fbx"], "alertSoft");
    avatar->emoteMotiono["angry"] = avatar->mixer->createMotion(animationo["angry.fbx"], "angry");
    avatar->emoteMotiono["angrySoft"] = avatar->mixer->createMotion(animationo["angry_soft.fbx"], "angrySoft");
    avatar->emoteMotiono["embarrassed"] = avatar->mixer->createMotion(animationo["embarrassed.fbx"], "embarrassed");
    avatar->emoteMotiono["embarrassedSoft"] = avatar->mixer->createMotion(animationo["embarrassed_soft.fbx"], "embarrassedSoft");
    avatar->emoteMotiono["headNod"] = avatar->mixer->createMotion(animationo["head_nod.fbx"], "headNod");
    avatar->emoteMotiono["headNodSoft"] = avatar->mixer->createMotion(animationo["head_nod_single.fbx"], "headNodSoft");
    avatar->emoteMotiono["headShake"] = avatar->mixer->createMotion(animationo["head_shake.fbx"], "headShake");
    avatar->emoteMotiono["headShakeSoft"] = avatar->mixer->createMotion(animationo["head_shake_single.fbx"], "headShakeSoft");
    avatar->emoteMotiono["sad"] = avatar->mixer->createMotion(animationo["sad.fbx"], "sad");
    avatar->emoteMotiono["sadSoft"] = avatar->mixer->createMotion(animationo["sad_soft.fbx"], "sadSoft");
    avatar->emoteMotiono["surprise"] = avatar->mixer->createMotion(animationo["surprise.fbx"], "surprise");
    avatar->emoteMotiono["surpriseSoft"] = avatar->mixer->createMotion(animationo["surprise_soft.fbx"], "surpriseSoft");
    avatar->emoteMotiono["victory"] = avatar->mixer->createMotion(animationo["victory.fbx"], "victory");
    avatar->emoteMotiono["victorySoft"] = avatar->mixer->createMotion(animationo["victory_soft.fbx"], "victorySoft");
    for (auto const& x : avatar->emoteMotiono)
    {
      avatar->emoteMotiono[x.first]->setLoop(LoopType::LoopOnce);
      avatar->emoteMotiono[x.first]->stop();
    }

    // danceAnimations
    avatar->danceMotiono["dansu"] = avatar->mixer->createMotion(animationo["Hip Hop Dancing.fbx"], "dansu");
    avatar->danceMotiono["powerup"] = avatar->mixer->createMotion(animationo["powerup.fbx"], "powerup");

    // holdAnimations
    avatar->holdMotiono["pick_up_idle"] = avatar->mixer->createMotion(animationo["pick_up_idle.fbx"], "pick_up_idle");

    // activateAnimations
    avatar->activateMotiono["grab_forward"] = avatar->mixer->createMotion(animationo["grab_forward.fbx"], "grab_forward");
    avatar->activateMotiono["grab_down"] = avatar->mixer->createMotion(animationo["grab_down.fbx"], "grab_down");
    avatar->activateMotiono["grab_up"] = avatar->mixer->createMotion(animationo["grab_up.fbx"], "grab_up");
    avatar->activateMotiono["grab_left"] = avatar->mixer->createMotion(animationo["grab_left.fbx"], "grab_left");
    avatar->activateMotiono["grab_right"] = avatar->mixer->createMotion(animationo["grab_right.fbx"], "grab_right");
    avatar->activateMotiono["pick_up"] = avatar->mixer->createMotion(animationo["pick_up.fbx"], "pick_up");
    for (auto const& x : avatar->activateMotiono)
    {
      avatar->activateMotiono[x.first]->setLoop(LoopType::LoopOnce);
      avatar->activateMotiono[x.first]->stop();
    }
    avatar->activateMotiono["grab_forward"]->setSpeed(1.2);
    avatar->activateMotiono["grab_down"]->setSpeed(1.7);
    avatar->activateMotiono["grab_up"]->setSpeed(1.2);
    avatar->activateMotiono["grab_left"]->setSpeed(1.2);
    avatar->activateMotiono["grab_right"]->setSpeed(1.2);
    avatar->activateMotiono["pick_up"]->setSpeed(1);

    //
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);
    // avatar->motiono[] = avatar->mixer->createMotion(animationo[]);

    // create node ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    avatar->nodeo["_8DirectionsWalkNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsWalkNodeList");
    addChild(avatar->nodeo["_8DirectionsWalkNodeList"], avatar->motiono["walkForward"]);
    addChild(avatar->nodeo["_8DirectionsWalkNodeList"], avatar->motiono["walkBackward"]);
    addChild(avatar->nodeo["_8DirectionsWalkNodeList"], avatar->motiono["walkLeft"]);
    addChild(avatar->nodeo["_8DirectionsWalkNodeList"], avatar->motiono["walkRight"]);
    addChild(avatar->nodeo["_8DirectionsWalkNodeList"], avatar->motiono["walkLeftMirror"]);
    addChild(avatar->nodeo["_8DirectionsWalkNodeList"], avatar->motiono["walkRightMirror"]);

    avatar->nodeo["_8DirectionsRunNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsRunNodeList");
    addChild(avatar->nodeo["_8DirectionsRunNodeList"], avatar->motiono["runForward"]);
    addChild(avatar->nodeo["_8DirectionsRunNodeList"], avatar->motiono["runBackward"]);
    addChild(avatar->nodeo["_8DirectionsRunNodeList"], avatar->motiono["runLeft"]);
    addChild(avatar->nodeo["_8DirectionsRunNodeList"], avatar->motiono["runRight"]);
    addChild(avatar->nodeo["_8DirectionsRunNodeList"], avatar->motiono["runLeftMirror"]);
    addChild(avatar->nodeo["_8DirectionsRunNodeList"], avatar->motiono["runRightMirror"]);

    avatar->nodeo["_8DirectionsCrouchNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsCrouchNodeList");
    addChild(avatar->nodeo["_8DirectionsCrouchNodeList"], avatar->motiono["crouchForward"]);
    addChild(avatar->nodeo["_8DirectionsCrouchNodeList"], avatar->motiono["crouchBackward"]);
    addChild(avatar->nodeo["_8DirectionsCrouchNodeList"], avatar->motiono["crouchLeft"]);
    addChild(avatar->nodeo["_8DirectionsCrouchNodeList"], avatar->motiono["crouchRight"]);
    addChild(avatar->nodeo["_8DirectionsCrouchNodeList"], avatar->motiono["crouchLeftMirror"]);
    addChild(avatar->nodeo["_8DirectionsCrouchNodeList"], avatar->motiono["crouchRightMirror"]);

    avatar->nodeo["_8DirectionsBowNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsBowNodeList");
    addChild(avatar->nodeo["_8DirectionsBowNodeList"], avatar->motiono["bowForward"]);
    addChild(avatar->nodeo["_8DirectionsBowNodeList"], avatar->motiono["bowBackward"]);
    addChild(avatar->nodeo["_8DirectionsBowNodeList"], avatar->motiono["bowLeft"]);
    addChild(avatar->nodeo["_8DirectionsBowNodeList"], avatar->motiono["bowRight"]);
    addChild(avatar->nodeo["_8DirectionsBowNodeList"], avatar->motiono["bowLeftMirror"]);
    addChild(avatar->nodeo["_8DirectionsBowNodeList"], avatar->motiono["bowRightMirror"]);

    avatar->nodeo["_8DirectionsWalkRunNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "_8DirectionsWalkRunNodeTwo");
    addChild(avatar->nodeo["_8DirectionsWalkRunNodeTwo"], avatar->nodeo["_8DirectionsWalkNodeList"]);
    addChild(avatar->nodeo["_8DirectionsWalkRunNodeTwo"], avatar->nodeo["_8DirectionsRunNodeList"]);

    avatar->nodeo["idle8DWalkRunNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DWalkRunNodeTwo");
    addChild(avatar->nodeo["idle8DWalkRunNodeTwo"], avatar->motiono["idle"]);
    addChild(avatar->nodeo["idle8DWalkRunNodeTwo"], avatar->nodeo["_8DirectionsWalkRunNodeTwo"]);

    avatar->nodeo["idle8DCrouchNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DCrouchNodeTwo");
    addChild(avatar->nodeo["idle8DCrouchNodeTwo"], avatar->motiono["crouchIdle"]);
    addChild(avatar->nodeo["idle8DCrouchNodeTwo"], avatar->nodeo["_8DirectionsCrouchNodeList"]);

    avatar->nodeo["flyForwardNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "flyForwardNodeTwo");
    addChild(avatar->nodeo["flyForwardNodeTwo"], avatar->motiono["flyDodgeForward"]);
    addChild(avatar->nodeo["flyForwardNodeTwo"], avatar->motiono["flyDash"]);

    avatar->nodeo["_8DirectionsFlyNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsFlyNodeList");
    addChild(avatar->nodeo["_8DirectionsFlyNodeList"], avatar->nodeo["flyForwardNodeTwo"]);
    addChild(avatar->nodeo["_8DirectionsFlyNodeList"], avatar->motiono["flyDodgeBackward"]);
    addChild(avatar->nodeo["_8DirectionsFlyNodeList"], avatar->motiono["flyDodgeLeft"]);
    addChild(avatar->nodeo["_8DirectionsFlyNodeList"], avatar->motiono["flyDodgeRight"]);

    avatar->nodeo["idle8DFlyNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DFlyNodeTwo");
    addChild(avatar->nodeo["idle8DFlyNodeTwo"], avatar->motiono["flyIdle"]);
    addChild(avatar->nodeo["idle8DFlyNodeTwo"], avatar->nodeo["_8DirectionsFlyNodeList"]);

    avatar->nodeo["idle8DBowNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DBowNodeTwo");
    addChild(avatar->nodeo["idle8DBowNodeTwo"], avatar->bowMotiono["bowIdle"]);
    addChild(avatar->nodeo["idle8DBowNodeTwo"], avatar->nodeo["_8DirectionsBowNodeList"]);

    avatar->nodeo["bowDrawLooseNodoeTwo"] = avatar->mixer->createNode(NodeType::TWO, "bowDrawLooseNodoeTwo");
    addChild(avatar->nodeo["bowDrawLooseNodoeTwo"], avatar->bowMotiono["bowDraw"]);
    addChild(avatar->nodeo["bowDrawLooseNodoeTwo"], avatar->bowMotiono["bowLoose"]);

    avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"] = avatar->mixer->createNode(NodeType::OVERWRITE, "bowIdle8DDrawLooseNodeOverwrite");
    addChild(avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"], avatar->nodeo["idle8DBowNodeTwo"]);
    addChild(avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"], avatar->nodeo["bowDrawLooseNodoeTwo"]);

    avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DWalkRun_BowIdle8DDrawLooseNodeTwo");
    addChild(avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"], avatar->nodeo["idle8DWalkRunNodeTwo"]);
    addChild(avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"], avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"]);

    avatar->nodeo["defaultNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "defaultNodeTwo");
    addChild(avatar->nodeo["defaultNodeTwo"], avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]);
    addChild(avatar->nodeo["defaultNodeTwo"], avatar->nodeo["idle8DCrouchNodeTwo"]);

    // hurtAnimations
    avatar->nodeo["hurtsNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "hurtsNodeSolitary");
    addChild(avatar->nodeo["hurtsNodeSolitary"], avatar->hurtMotiono["pain_back"]);
    addChild(avatar->nodeo["hurtsNodeSolitary"], avatar->hurtMotiono["pain_arch"]);
    //
    avatar->nodeo["hurtNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "hurtNodeTwo");
    addChild(avatar->nodeo["hurtNodeTwo"], avatar->nodeo["defaultNodeTwo"]);
    addChild(avatar->nodeo["hurtNodeTwo"], avatar->nodeo["hurtsNodeSolitary"]);

    // useAnimations
    avatar->nodeo["usesNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "usesNodeSolitary");
    // https://stackoverflow.com/questions/26281979/c-loop-through-map
    for (auto const& x : avatar->useMotiono)
    {
      std::cout << "useMotiono: " << avatar->useMotiono[x.first]->name << std::endl;
      addChild(avatar->nodeo["usesNodeSolitary"], avatar->useMotiono[x.first]);
    }
    //
    avatar->nodeo["useNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "useNodeTwo");
    addChild(avatar->nodeo["useNodeTwo"], avatar->nodeo["hurtNodeTwo"]);
    addChild(avatar->nodeo["useNodeTwo"], avatar->nodeo["usesNodeSolitary"]);

    // useComboAnimations
    avatar->nodeo["useCombosNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "useCombosNodeSolitary");
    //
    addChild(avatar->nodeo["useCombosNodeSolitary"], avatar->nodeo["useNodeTwo"]);
    //
    for (auto const& x : avatar->useComboMotiono)
    {
      addChild(avatar->nodeo["useCombosNodeSolitary"], avatar->useComboMotiono[x.first]);
    }

    // emoteAnimations
    avatar->nodeo["emotesNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "emotesNodeSolitary");
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["alert"]); // todo: for loop
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["alertSoft"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["angry"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["angrySoft"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["embarrassed"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["embarrassedSoft"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["headNod"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["headNodSoft"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["headShake"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["headShakeSoft"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["sad"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["sadSoft"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["surprise"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["surpriseSoft"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["victory"]);
    addChild(avatar->nodeo["emotesNodeSolitary"], avatar->emoteMotiono["victorySoft"]);
    //
    avatar->nodeo["emoteNodeFunc"] = avatar->mixer->createNode(NodeType::FUNC, "emoteNodeFunc", 1);
    addChild(avatar->nodeo["emoteNodeFunc"], avatar->nodeo["useCombosNodeSolitary"]);
    addChild(avatar->nodeo["emoteNodeFunc"], avatar->nodeo["emotesNodeSolitary"]);

    // danceAnimations
    avatar->nodeo["dancesNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "dancesNodeSolitary");
    addChild(avatar->nodeo["dancesNodeSolitary"], avatar->danceMotiono["dansu"]);
    addChild(avatar->nodeo["dancesNodeSolitary"], avatar->danceMotiono["powerup"]);
    //
    avatar->nodeo["danceNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "danceNodeTwo");
    addChild(avatar->nodeo["danceNodeTwo"], avatar->nodeo["emoteNodeFunc"]);
    addChild(avatar->nodeo["danceNodeTwo"], avatar->nodeo["dancesNodeSolitary"]);

    avatar->nodeo["narutoRunNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "narutoRunNodeTwo");
    addChild(avatar->nodeo["narutoRunNodeTwo"], avatar->nodeo["danceNodeTwo"]);
    addChild(avatar->nodeo["narutoRunNodeTwo"], avatar->motiono["narutoRun"]);

    // sitAnimations
    avatar->nodeo["sitsNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "sitsNodeSolitary");
    addChild(avatar->nodeo["sitsNodeSolitary"], avatar->sitMotiono["chair"]);
    addChild(avatar->nodeo["sitsNodeSolitary"], avatar->sitMotiono["saddle"]);
    addChild(avatar->nodeo["sitsNodeSolitary"], avatar->sitMotiono["stand"]);
    //
    avatar->nodeo["sitNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "sitNodeTwo");
    addChild(avatar->nodeo["sitNodeTwo"], avatar->nodeo["narutoRunNodeTwo"]);
    addChild(avatar->nodeo["sitNodeTwo"], avatar->nodeo["sitsNodeSolitary"]);

    avatar->nodeo["jumpNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "jumpNodeTwo");
    addChild(avatar->nodeo["jumpNodeTwo"], avatar->nodeo["sitNodeTwo"]);
    addChild(avatar->nodeo["jumpNodeTwo"], avatar->motiono["jump"]);

    avatar->nodeo["doubleJumpNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "doubleJumpNodeTwo");
    addChild(avatar->nodeo["doubleJumpNodeTwo"], avatar->nodeo["jumpNodeTwo"]);
    addChild(avatar->nodeo["doubleJumpNodeTwo"], avatar->motiono["doubleJump"]);

    avatar->nodeo["groundFlyNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "groundFlyNodeTwo");
    addChild(avatar->nodeo["groundFlyNodeTwo"], avatar->nodeo["doubleJumpNodeTwo"]);
    addChild(avatar->nodeo["groundFlyNodeTwo"], avatar->nodeo["idle8DFlyNodeTwo"]);

    avatar->nodeo["fallLoopNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "fallLoopNodeTwo");
    addChild(avatar->nodeo["fallLoopNodeTwo"], avatar->nodeo["groundFlyNodeTwo"]);
    addChild(avatar->nodeo["fallLoopNodeTwo"], avatar->motiono["fallLoop"]);

    avatar->nodeo["landsNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "landsNodeSolitary");
    addChild(avatar->nodeo["landsNodeSolitary"], avatar->motiono["land"]);
    addChild(avatar->nodeo["landsNodeSolitary"], avatar->motiono["land2"]);
    //
    avatar->nodeo["landNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "landNodeTwo");
    addChild(avatar->nodeo["landNodeTwo"], avatar->nodeo["fallLoopNodeTwo"]);
    addChild(avatar->nodeo["landNodeTwo"], avatar->nodeo["landsNodeSolitary"]);

    // activateAnimations
    avatar->nodeo["activatesNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "activatesNodeSolitary");
    addChild(avatar->nodeo["activatesNodeSolitary"], avatar->activateMotiono["grab_forward"]);
    addChild(avatar->nodeo["activatesNodeSolitary"], avatar->activateMotiono["grab_down"]);
    addChild(avatar->nodeo["activatesNodeSolitary"], avatar->activateMotiono["grab_up"]);
    addChild(avatar->nodeo["activatesNodeSolitary"], avatar->activateMotiono["grab_left"]);
    addChild(avatar->nodeo["activatesNodeSolitary"], avatar->activateMotiono["grab_right"]);
    addChild(avatar->nodeo["activatesNodeSolitary"], avatar->activateMotiono["pick_up"]);
    //
    avatar->nodeo["activateNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "activateNodeTwo");
    addChild(avatar->nodeo["activateNodeTwo"], avatar->nodeo["landNodeTwo"]);
    addChild(avatar->nodeo["activateNodeTwo"], avatar->nodeo["activatesNodeSolitary"]);

    // holdAnimations
    avatar->nodeo["holdsNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "holdsNodeSolitary");
    addChild(avatar->nodeo["holdsNodeSolitary"], avatar->holdMotiono["pick_up_idle"]);
    //
    avatar->nodeo["holdNodeFunc"] = avatar->mixer->createNode(NodeType::FUNC, "holdNodeFunc", 0);
    addChild(avatar->nodeo["holdNodeFunc"], avatar->nodeo["activateNodeTwo"]);
    addChild(avatar->nodeo["holdNodeFunc"], avatar->nodeo["holdsNodeSolitary"]);

    // set root node ---
    setRootNode(avatar->mixer, avatar->nodeo["holdNodeFunc"]);

    return avatar;
  }
  void updateAvatarString(Avatar *avatar, char *scratchStack, unsigned int numStrings)
  {
    avatar->strings.clear();
    unsigned int index = 0;
    for (unsigned int stringIndex = 0; stringIndex < numStrings; stringIndex++)
    {
      unsigned int stringByteLength = (unsigned int)(scratchStack[index++]);
      std::string string = "";
      for (unsigned int charIndex = 0; charIndex < stringByteLength; charIndex++)
      {
        string += scratchStack[index++];
      }
      avatar->strings.push_back(string);
      // avatar->strings[stringIndex] = string;
      // std::cout << string << std::endl;
    }
  }
  void updateAvatar(Avatar *avatar, float *scratchStack)
  {
    // std::cout << "idleWalkFactor: " << scratchStack[index] << std::endl;

    unsigned int index = 0;

    // strings ----
    std::string defaultSitAnimation = avatar->strings[index++];
    std::string defaultEmoteAnimation = avatar->strings[index++];
    std::string defaultDanceAnimation = avatar->strings[index++];
    std::string defaultHoldAnimation = avatar->strings[index++];
    std::string defaultActivateAnimation = avatar->strings[index++];
    // ---
    std::string useAnimationComboName = avatar->strings[index++];
    std::string sitAnimation = avatar->strings[index++];
    std::string emoteAnimation = avatar->strings[index++];
    std::string danceAnimation = avatar->strings[index++];
    std::string holdAnimation = avatar->strings[index++];
    std::string activateAnimation = avatar->strings[index++];
    std::string hurtAnimation = avatar->strings[index++];

    // std::cout << "sitAnimation: " << sitAnimation << std::endl;

    // ---------------------------------------------------------------------------------------------------

    index = 0;

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
    float useEnvelopeState = scratchStack[index++];

    // values ---
    setWeight(avatar->motiono["walkForward"], forwardFactor);
    setWeight(avatar->motiono["walkBackward"], backwardFactor);
    setWeight(avatar->motiono["walkLeft"], mirrorLeftFactorReverse);
    setWeight(avatar->motiono["walkLeftMirror"], mirrorLeftFactor);
    setWeight(avatar->motiono["walkRight"], mirrorRightFactorReverse);
    setWeight(avatar->motiono["walkRightMirror"], mirrorRightFactor);

    setWeight(avatar->motiono["runForward"], forwardFactor);
    setWeight(avatar->motiono["runBackward"], backwardFactor);
    setWeight(avatar->motiono["runLeft"], mirrorLeftFactorReverse);
    setWeight(avatar->motiono["runLeftMirror"], mirrorLeftFactor);
    setWeight(avatar->motiono["runRight"], mirrorRightFactorReverse);
    setWeight(avatar->motiono["runRightMirror"], mirrorRightFactor);

    setWeight(avatar->motiono["crouchForward"], forwardFactor);
    setWeight(avatar->motiono["crouchBackward"], backwardFactor);
    setWeight(avatar->motiono["crouchLeft"], mirrorLeftFactorReverse);
    setWeight(avatar->motiono["crouchLeftMirror"], mirrorLeftFactor);
    setWeight(avatar->motiono["crouchRight"], mirrorRightFactorReverse);
    setWeight(avatar->motiono["crouchRightMirror"], mirrorRightFactor);

    setWeight(avatar->motiono["bowForward"], forwardFactor);
    setWeight(avatar->motiono["bowBackward"], backwardFactor);
    setWeight(avatar->motiono["bowLeft"], mirrorLeftFactorReverse);
    setWeight(avatar->motiono["bowLeftMirror"], mirrorLeftFactor);
    setWeight(avatar->motiono["bowRight"], mirrorRightFactorReverse);
    setWeight(avatar->motiono["bowRightMirror"], mirrorRightFactor);

    setFactor(avatar->nodeo["_8DirectionsWalkRunNodeTwo"], walkRunFactor);
    setFactor(avatar->nodeo["idle8DWalkRunNodeTwo"], idleWalkFactor);
    setFactor(avatar->nodeo["idle8DCrouchNodeTwo"], idleWalkFactor);
    setFactor(avatar->nodeo["defaultNodeTwo"], crouchFactor);
    setFactor(avatar->nodeo["idle8DBowNodeTwo"], idleWalkFactor);

    setWeight(avatar->nodeo["flyForwardNodeTwo"], forwardFactor);
    setWeight(avatar->motiono["flyDodgeBackward"], backwardFactor);
    setWeight(avatar->motiono["flyDodgeLeft"], leftFactor);
    setWeight(avatar->motiono["flyDodgeRight"], rightFactor);

    setFactor(avatar->nodeo["idle8DFlyNodeTwo"], walkRunFactor);
    setFactor(avatar->nodeo["flyForwardNodeTwo"], flyDashFactor);

    setArg(avatar->nodeo["holdNodeFunc"], holdFactor);
    setArg(avatar->nodeo["emoteNodeFunc"], idleWalkFactor);

    // action end events ---
    if (landEnd) {
      if (!landWithMoving) {
        avatar->nodeo["landNodeTwo"]->crossFadeTwo(0.05, 0);
      } else {
        avatar->nodeo["landNodeTwo"]->crossFadeTwo(0.15, 0);
      }
    }
    
    if (fallLoopEnd) {
      avatar->nodeo["fallLoopNodeTwo"]->crossFadeTwo(0.2, 0);
    }
    
    if (flyEnd) {
      avatar->nodeo["groundFlyNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (jumpEnd) {
      avatar->nodeo["jumpNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (doubleJumpEnd) {
      avatar->nodeo["doubleJumpNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (narutoRunEnd) {
      avatar->nodeo["narutoRunNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (activateEnd) {
      avatar->nodeo["activateNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (useEnd) {
      avatar->nodeo["useNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (useComboEnd) {
      avatar->nodeo["useCombosNodeSolitary"]->crossFadeSolitary(0.2, avatar->nodeo["useNodeTwo"]);
    }

    if (useEnvelopeEnd) {
      avatar->bowMotiono["bowLoose"]->play();
      setFactor(avatar->nodeo["bowDrawLooseNodoeTwo"], 1);
      avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->crossFadeTwo(0.2, 1);
    }

    if (sitEnd) {
      avatar->nodeo["sitNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (emoteEnd) {
      avatar->nodeo["emoteNodeFunc"]->crossFadeTwo(0.2, 0);
    }

    if (hurtEnd) {
      avatar->nodeo["hurtNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (danceEnd) {
      avatar->nodeo["danceNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (holdEnd) {
      avatar->nodeo["holdNodeFunc"]->crossFadeTwo(0.2, 0);
    }

    // action start events ---
    if (landStart) {
      // std::cout << "landWithMoving: " << landWithMoving << std::endl;
      // std::cout << "landWithMoving: " << (bool)(landWithMoving) << std::endl;
      if (!landWithMoving) {
        avatar->motiono["land"]->play();
        avatar->nodeo["landsNodeSolitary"]->crossFadeSolitary(0, avatar->motiono["land"]);
        avatar->nodeo["landNodeTwo"]->crossFadeTwo(0, 1);
      } else {
        avatar->motiono["land2"]->play();
        avatar->nodeo["landsNodeSolitary"]->crossFadeSolitary(0, avatar->motiono["land2"]);
        avatar->nodeo["landNodeTwo"]->crossFadeTwo(0.1, 1);
      }
    }

    if (fallLoopStart) {
      avatar->nodeo["fallLoopNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (flyStart) {
      avatar->nodeo["groundFlyNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (jumpStart) {
      avatar->motiono["jump"]->play();
      avatar->nodeo["jumpNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (doubleJumpStart) {
      avatar->motiono["doubleJump"]->play();
      avatar->nodeo["doubleJumpNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (narutoRunStart) {
      avatar->nodeo["narutoRunNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // useAnimations // sword
    if (useStart) {
      // let animationName;
      // if (dashAttacking) {
      //   animationName = 'dashAttack';
      // } else {
      //   animationName = avatar->useAnimation;
      // }
      std::string animationName = "combo"; // test
      // AnimationNode *useMotion = avatar->useMotiono[animationName];
      avatar->useMotiono[animationName]->play();
      avatar->nodeo["usesNodeSolitary"]->crossFadeSolitary(0, avatar->useMotiono[animationName]);
      avatar->nodeo["useNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // useComboAnimations // silsword
    if (useComboStart) {
      std::string animationName;
      if (dashAttacking) {
        animationName = "dashAttack";
      } else {
        // animationName = avatar->useAnimationCombo[avatar->useAnimationIndex];
        // animationName = "swordSideSlash"; // test
        animationName = useAnimationComboName;
        std::cout << "useAnimationComboName: " << useAnimationComboName << std::endl;
      }
      avatar->useComboMotiono[animationName]->play();
      avatar->nodeo["useCombosNodeSolitary"]->crossFadeSolitary(0.2, avatar->useComboMotiono[animationName]);
    }

    // bow
    if (useEnvelopeStart) {
      avatar->bowMotiono["bowDraw"]->play();
      setFactor(avatar->nodeo["bowDrawLooseNodoeTwo"], 0);
      setFactor(avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"], 1);
      avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // sitAnimations // sit
    if (sitStart) {
      // AnimationNode *sitMotion = avatar->sitMotiono[sitAnimation == "" ? defaultSitAnimation : sitAnimation];
      std::string animationName = sitAnimation == "" ? defaultSitAnimation : sitAnimation;
      std::cout << "animationName: " << animationName << std::endl;
      avatar->sitMotiono[animationName]->play();
      avatar->nodeo["sitsNodeSolitary"]->crossFadeSolitary(0, avatar->sitMotiono[animationName]);
      avatar->nodeo["sitNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // emoteAnimations // emote
    if (emoteStart) {
      AnimationNode *emoteMotion = avatar->emoteMotiono[emoteAnimation == "" ? defaultEmoteAnimation : emoteAnimation];
      emoteMotion->play();
      avatar->nodeo["emotesNodeSolitary"]->crossFadeSolitary(0, emoteMotion);
      avatar->nodeo["emoteNodeFunc"]->crossFadeTwo(0.2, 1);
      // physx.physxWorker.setFactor(avatar->emoteNodeFuncPtr, 1);
    }

    // hurtAnimations // hurt
    if (hurtStart) {
      AnimationNode *hurtMotion = avatar->hurtMotiono[hurtAnimation];
      hurtMotion->play();
      avatar->nodeo["hurtsNodeSolitary"]->crossFadeSolitary(0, hurtMotion);
      avatar->nodeo["hurtNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // danceAnimations // dance
    if (danceStart) {
      AnimationNode *danceMotion = avatar->danceMotiono[danceAnimation == "" ? defaultDanceAnimation : danceAnimation];
      danceMotion->play();
      avatar->nodeo["dancesNodeSolitary"]->crossFadeSolitary(0, danceMotion);
      avatar->nodeo["danceNodeTwo"]->crossFadeTwo(0.2, 1); // todo: check if node exists
    }

    // holdAnimations // hold
    if (holdStart) {
      AnimationNode *holdMotion = avatar->holdMotiono[holdAnimation == "" ? defaultHoldAnimation : holdAnimation];
      holdMotion->play();
      avatar->nodeo["holdsNodeSolitary"]->crossFadeSolitary(0, holdMotion);
      avatar->nodeo["holdNodeFunc"]->crossFadeTwo(0.2, 1);
    }

    // activateAnimations // activate
    if (activateStart) {
      AnimationNode *activateMotion = avatar->activateMotiono[activateAnimation == "" ? defaultActivateAnimation : activateAnimation];
      activateMotion->play();
      avatar->nodeo["activatesNodeSolitary"]->crossFadeSolitary(0, activateMotion);
      avatar->nodeo["activateNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // handle finished event ---
    if (avatar->mixer->finishedFlag)
    {
      std::string finishedMotionName = avatar->mixer->finishedMotion->name;
      if (useEnvelopeState && finishedMotionName == "bowDraw") {
        avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->crossFadeTwo(0.2, 0);
      }
      if (finishedMotionName == "bowLoose") {
        avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->crossFadeTwo(0.2, 0);
      }
    }
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
  void addChild(AnimationNode *parent, AnimationNode *child) // todo: check if node exists
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
    animation->name = name; // todo: don't need ?

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
  AnimationNode *AnimationMixer::createNode(NodeType type, std::string name, unsigned int index)
  {
    AnimationNode *node = new AnimationNode();
    node->mixer = this;
    node->type = type;
    node->name = name;
    // std::cout << "NodeType: " << type << " " << node->type << std::endl;
    
    node->index = index; // note: only for NodeType::FUNC now

    this->nodeo[name] = node;

    return node;
  }
  AnimationNode *AnimationMixer::createMotion(Animation *animation, std::string name)
  {
    AnimationNode *motion = new AnimationNode();
    motion->mixer = this;
    motion->animation = animation;
    motion->name = name;

    // this->motions.push_back(motion);
    this->motiono[name] = motion;

    return motion;
  }
  AnimationNode *AnimationMixer::getMotion(char *scratchStack, unsigned int nameByteLength)
  {
    std::string name = "";
    for (unsigned int i = 0; i < nameByteLength; i++)
    {
      name += scratchStack[i];
    }

    return this->motiono[name];
  }
  unsigned int AnimationMixer::getMotionName(AnimationNode *motion, char *scratchStack)
  {
    unsigned int nameByteLength = motion->name.length();
    for (unsigned int i = 0; i < nameByteLength; i++)
    {
      scratchStack[i] = motion->name.at(i);
    }
    return nameByteLength;
  }
  AnimationNode *AnimationMixer::getNode(char *scratchStack, unsigned int nameByteLength)
  {
    std::string name = "";
    for (unsigned int i = 0; i < nameByteLength; i++)
    {
      name += scratchStack[i];
    }

    return this->nodeo[name];
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
    this->finishedMotion = NULL;

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
  unsigned int AnimationMixer::getFinishedMotionName(char *scratchStack) // todo: getFinishedMotionNames
  {
    unsigned int nameByteLength = this->finishedMotion->name.length();
    for (unsigned int i = 0; i < nameByteLength; i++)
    {
      scratchStack[i] = this->finishedMotion->name.at(i);
    }
    return nameByteLength;
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
        // if (spec.isPosition && this->name == "jump") std::cout << "evaluateTimeS: " << evaluateTimeS << std::endl;
        // if (spec.isPosition && this->name == "swordSideSlash") std::cout << "evaluateTimeS: " << evaluateTimeS << std::endl;
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
          // for (int i = 0; i < this->mixer->motions.size(); i++)
          for (auto const& x : this->mixer->motiono)
          {
            // AnimationNode *motion = this->mixer->motions[i];
            AnimationNode *motion = this->mixer->motiono[x.first];
            // std::cout << "---wasm: " << motion->name << std::endl;
            if (motion->animation == animation)
            {
              // std::cout << "finished: motion: pointer: " << motion << " pointer float: " << (float *)motion << std::endl;
              // this->mixer->animationValues[54] = (float *)motion; // finished motion pointer
              this->mixer->finishedMotion = motion;
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
    // todo: check if motion exists
    std::cout << "play name: " << this->name << std::endl;
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