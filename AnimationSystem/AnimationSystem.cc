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

  void AnimationNode::setWeight(float weight)
  {
    // if (node == NULL) std::cout << "no node" << std::endl;
    // if (node->weight == NULL) std::cout << "no weight" << std::endl;
    this->weight = weight;
  }

  void AnimationNode::setFactor(float factor)
  {
    this->factor = factor;
  }

  void AnimationNode::setArg(float arg)
  {
    this->arg = arg;
  }

  float AnimationNode::getWeight()
  {
    return this->weight;
  }

  float AnimationNode::getFactor()
  {
    return this->factor;
  }

  unsigned int AnimationNode::getChildren(AnimationNode **scratchStack) {
    unsigned int count = this->children.size();
    for (unsigned int i = 0; i < count; i++) {
      scratchStack[i] = this->children[i];
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

  Avatar *createAvatar(AnimationMixer *mixer) {
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
    avatar->nodeo["_8DirectionsWalkNodeList"]->addChild(avatar->motiono["walkForward"]);
    avatar->nodeo["_8DirectionsWalkNodeList"]->addChild(avatar->motiono["walkBackward"]);
    avatar->nodeo["_8DirectionsWalkNodeList"]->addChild(avatar->motiono["walkLeft"]);
    avatar->nodeo["_8DirectionsWalkNodeList"]->addChild(avatar->motiono["walkRight"]);
    avatar->nodeo["_8DirectionsWalkNodeList"]->addChild(avatar->motiono["walkLeftMirror"]);
    avatar->nodeo["_8DirectionsWalkNodeList"]->addChild(avatar->motiono["walkRightMirror"]);

    avatar->nodeo["_8DirectionsRunNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsRunNodeList");
    avatar->nodeo["_8DirectionsRunNodeList"]->addChild(avatar->motiono["runForward"]);
    avatar->nodeo["_8DirectionsRunNodeList"]->addChild(avatar->motiono["runBackward"]);
    avatar->nodeo["_8DirectionsRunNodeList"]->addChild(avatar->motiono["runLeft"]);
    avatar->nodeo["_8DirectionsRunNodeList"]->addChild(avatar->motiono["runRight"]);
    avatar->nodeo["_8DirectionsRunNodeList"]->addChild(avatar->motiono["runLeftMirror"]);
    avatar->nodeo["_8DirectionsRunNodeList"]->addChild(avatar->motiono["runRightMirror"]);

    avatar->nodeo["_8DirectionsCrouchNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsCrouchNodeList");
    avatar->nodeo["_8DirectionsCrouchNodeList"]->addChild(avatar->motiono["crouchForward"]);
    avatar->nodeo["_8DirectionsCrouchNodeList"]->addChild(avatar->motiono["crouchBackward"]);
    avatar->nodeo["_8DirectionsCrouchNodeList"]->addChild(avatar->motiono["crouchLeft"]);
    avatar->nodeo["_8DirectionsCrouchNodeList"]->addChild(avatar->motiono["crouchRight"]);
    avatar->nodeo["_8DirectionsCrouchNodeList"]->addChild(avatar->motiono["crouchLeftMirror"]);
    avatar->nodeo["_8DirectionsCrouchNodeList"]->addChild(avatar->motiono["crouchRightMirror"]);

    avatar->nodeo["_8DirectionsBowNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsBowNodeList");
    avatar->nodeo["_8DirectionsBowNodeList"]->addChild(avatar->motiono["bowForward"]);
    avatar->nodeo["_8DirectionsBowNodeList"]->addChild(avatar->motiono["bowBackward"]);
    avatar->nodeo["_8DirectionsBowNodeList"]->addChild(avatar->motiono["bowLeft"]);
    avatar->nodeo["_8DirectionsBowNodeList"]->addChild(avatar->motiono["bowRight"]);
    avatar->nodeo["_8DirectionsBowNodeList"]->addChild(avatar->motiono["bowLeftMirror"]);
    avatar->nodeo["_8DirectionsBowNodeList"]->addChild(avatar->motiono["bowRightMirror"]);

    avatar->nodeo["_8DirectionsWalkRunNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "_8DirectionsWalkRunNodeTwo");
    avatar->nodeo["_8DirectionsWalkRunNodeTwo"]->addChild(avatar->nodeo["_8DirectionsWalkNodeList"]);
    avatar->nodeo["_8DirectionsWalkRunNodeTwo"]->addChild(avatar->nodeo["_8DirectionsRunNodeList"]);

    avatar->nodeo["idle8DWalkRunNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DWalkRunNodeTwo");
    avatar->nodeo["idle8DWalkRunNodeTwo"]->addChild(avatar->motiono["idle"]);
    avatar->nodeo["idle8DWalkRunNodeTwo"]->addChild(avatar->nodeo["_8DirectionsWalkRunNodeTwo"]);

    avatar->nodeo["idle8DCrouchNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DCrouchNodeTwo");
    avatar->nodeo["idle8DCrouchNodeTwo"]->addChild(avatar->motiono["crouchIdle"]);
    avatar->nodeo["idle8DCrouchNodeTwo"]->addChild(avatar->nodeo["_8DirectionsCrouchNodeList"]);

    avatar->nodeo["flyForwardNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "flyForwardNodeTwo");
    avatar->nodeo["flyForwardNodeTwo"]->addChild(avatar->motiono["flyDodgeForward"]);
    avatar->nodeo["flyForwardNodeTwo"]->addChild(avatar->motiono["flyDash"]);

    avatar->nodeo["_8DirectionsFlyNodeList"] = avatar->mixer->createNode(NodeType::LIST, "_8DirectionsFlyNodeList");
    avatar->nodeo["_8DirectionsFlyNodeList"]->addChild(avatar->nodeo["flyForwardNodeTwo"]);
    avatar->nodeo["_8DirectionsFlyNodeList"]->addChild(avatar->motiono["flyDodgeBackward"]);
    avatar->nodeo["_8DirectionsFlyNodeList"]->addChild(avatar->motiono["flyDodgeLeft"]);
    avatar->nodeo["_8DirectionsFlyNodeList"]->addChild(avatar->motiono["flyDodgeRight"]);

    avatar->nodeo["idle8DFlyNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DFlyNodeTwo");
    avatar->nodeo["idle8DFlyNodeTwo"]->addChild(avatar->motiono["flyIdle"]);
    avatar->nodeo["idle8DFlyNodeTwo"]->addChild(avatar->nodeo["_8DirectionsFlyNodeList"]);

    avatar->nodeo["idle8DBowNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DBowNodeTwo");
    avatar->nodeo["idle8DBowNodeTwo"]->addChild(avatar->bowMotiono["bowIdle"]);
    avatar->nodeo["idle8DBowNodeTwo"]->addChild(avatar->nodeo["_8DirectionsBowNodeList"]);

    avatar->nodeo["bowDrawLooseNodoeTwo"] = avatar->mixer->createNode(NodeType::TWO, "bowDrawLooseNodoeTwo");
    avatar->nodeo["bowDrawLooseNodoeTwo"]->addChild(avatar->bowMotiono["bowDraw"]);
    avatar->nodeo["bowDrawLooseNodoeTwo"]->addChild(avatar->bowMotiono["bowLoose"]);

    avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"] = avatar->mixer->createNode(NodeType::OVERWRITE, "bowIdle8DDrawLooseNodeOverwrite");
    avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->addChild(avatar->nodeo["idle8DBowNodeTwo"]);
    avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->addChild(avatar->nodeo["bowDrawLooseNodoeTwo"]);

    avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "idle8DWalkRun_BowIdle8DDrawLooseNodeTwo");
    avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->addChild(avatar->nodeo["idle8DWalkRunNodeTwo"]);
    avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->addChild(avatar->nodeo["bowIdle8DDrawLooseNodeOverwrite"]);

    avatar->nodeo["defaultNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "defaultNodeTwo");
    avatar->nodeo["defaultNodeTwo"]->addChild(avatar->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]);
    avatar->nodeo["defaultNodeTwo"]->addChild(avatar->nodeo["idle8DCrouchNodeTwo"]);

    // hurtAnimations
    avatar->nodeo["hurtsNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "hurtsNodeSolitary");
    avatar->nodeo["hurtsNodeSolitary"]->addChild(avatar->hurtMotiono["pain_back"]);
    avatar->nodeo["hurtsNodeSolitary"]->addChild(avatar->hurtMotiono["pain_arch"]);
    //
    avatar->nodeo["hurtNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "hurtNodeTwo");
    avatar->nodeo["hurtNodeTwo"]->addChild(avatar->nodeo["defaultNodeTwo"]);
    avatar->nodeo["hurtNodeTwo"]->addChild(avatar->nodeo["hurtsNodeSolitary"]);

    // useAnimations
    avatar->nodeo["usesNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "usesNodeSolitary");
    // https://stackoverflow.com/questions/26281979/c-loop-through-map
    for (auto const& x : avatar->useMotiono)
    {
      std::cout << "useMotiono: " << avatar->useMotiono[x.first]->name << std::endl;
      avatar->nodeo["usesNodeSolitary"]->addChild(avatar->useMotiono[x.first]);
    }
    //
    avatar->nodeo["useNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "useNodeTwo");
    avatar->nodeo["useNodeTwo"]->addChild(avatar->nodeo["hurtNodeTwo"]);
    avatar->nodeo["useNodeTwo"]->addChild(avatar->nodeo["usesNodeSolitary"]);

    // useComboAnimations
    avatar->nodeo["useCombosNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "useCombosNodeSolitary");
    //
    avatar->nodeo["useCombosNodeSolitary"]->addChild(avatar->nodeo["useNodeTwo"]);
    //
    for (auto const& x : avatar->useComboMotiono)
    {
      avatar->nodeo["useCombosNodeSolitary"]->addChild(avatar->useComboMotiono[x.first]);
    }

    // emoteAnimations
    avatar->nodeo["emotesNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "emotesNodeSolitary");
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["alert"]); // todo: for loop
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["alertSoft"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["angry"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["angrySoft"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["embarrassed"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["embarrassedSoft"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["headNod"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["headNodSoft"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["headShake"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["headShakeSoft"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["sad"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["sadSoft"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["surprise"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["surpriseSoft"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["victory"]);
    avatar->nodeo["emotesNodeSolitary"]->addChild(avatar->emoteMotiono["victorySoft"]);
    //
    avatar->nodeo["emoteNodeFunc"] = avatar->mixer->createNode(NodeType::FUNC, "emoteNodeFunc", 1);
    avatar->nodeo["emoteNodeFunc"]->addChild(avatar->nodeo["useCombosNodeSolitary"]);
    avatar->nodeo["emoteNodeFunc"]->addChild(avatar->nodeo["emotesNodeSolitary"]);

    // danceAnimations
    avatar->nodeo["dancesNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "dancesNodeSolitary");
    avatar->nodeo["dancesNodeSolitary"]->addChild(avatar->danceMotiono["dansu"]);
    avatar->nodeo["dancesNodeSolitary"]->addChild(avatar->danceMotiono["powerup"]);
    //
    avatar->nodeo["danceNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "danceNodeTwo");
    avatar->nodeo["danceNodeTwo"]->addChild(avatar->nodeo["emoteNodeFunc"]);
    avatar->nodeo["danceNodeTwo"]->addChild(avatar->nodeo["dancesNodeSolitary"]);

    avatar->nodeo["narutoRunNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "narutoRunNodeTwo");
    avatar->nodeo["narutoRunNodeTwo"]->addChild(avatar->nodeo["danceNodeTwo"]);
    avatar->nodeo["narutoRunNodeTwo"]->addChild(avatar->motiono["narutoRun"]);

    // sitAnimations
    avatar->nodeo["sitsNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "sitsNodeSolitary");
    avatar->nodeo["sitsNodeSolitary"]->addChild(avatar->sitMotiono["chair"]);
    avatar->nodeo["sitsNodeSolitary"]->addChild(avatar->sitMotiono["saddle"]);
    avatar->nodeo["sitsNodeSolitary"]->addChild(avatar->sitMotiono["stand"]);
    //
    avatar->nodeo["sitNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "sitNodeTwo");
    avatar->nodeo["sitNodeTwo"]->addChild(avatar->nodeo["narutoRunNodeTwo"]);
    avatar->nodeo["sitNodeTwo"]->addChild(avatar->nodeo["sitsNodeSolitary"]);

    avatar->nodeo["jumpNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "jumpNodeTwo");
    avatar->nodeo["jumpNodeTwo"]->addChild(avatar->nodeo["sitNodeTwo"]);
    avatar->nodeo["jumpNodeTwo"]->addChild(avatar->motiono["jump"]);

    avatar->nodeo["doubleJumpNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "doubleJumpNodeTwo");
    avatar->nodeo["doubleJumpNodeTwo"]->addChild(avatar->nodeo["jumpNodeTwo"]);
    avatar->nodeo["doubleJumpNodeTwo"]->addChild(avatar->motiono["doubleJump"]);

    avatar->nodeo["groundFlyNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "groundFlyNodeTwo");
    avatar->nodeo["groundFlyNodeTwo"]->addChild(avatar->nodeo["doubleJumpNodeTwo"]);
    avatar->nodeo["groundFlyNodeTwo"]->addChild(avatar->nodeo["idle8DFlyNodeTwo"]);

    avatar->nodeo["fallLoopNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "fallLoopNodeTwo");
    avatar->nodeo["fallLoopNodeTwo"]->addChild(avatar->nodeo["groundFlyNodeTwo"]);
    avatar->nodeo["fallLoopNodeTwo"]->addChild(avatar->motiono["fallLoop"]);

    avatar->nodeo["landsNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "landsNodeSolitary");
    avatar->nodeo["landsNodeSolitary"]->addChild(avatar->motiono["land"]);
    avatar->nodeo["landsNodeSolitary"]->addChild(avatar->motiono["land2"]);
    //
    avatar->nodeo["landNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "landNodeTwo");
    avatar->nodeo["landNodeTwo"]->addChild(avatar->nodeo["fallLoopNodeTwo"]);
    avatar->nodeo["landNodeTwo"]->addChild(avatar->nodeo["landsNodeSolitary"]);

    // activateAnimations
    avatar->nodeo["activatesNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "activatesNodeSolitary");
    avatar->nodeo["activatesNodeSolitary"]->addChild(avatar->activateMotiono["grab_forward"]);
    avatar->nodeo["activatesNodeSolitary"]->addChild(avatar->activateMotiono["grab_down"]);
    avatar->nodeo["activatesNodeSolitary"]->addChild(avatar->activateMotiono["grab_up"]);
    avatar->nodeo["activatesNodeSolitary"]->addChild(avatar->activateMotiono["grab_left"]);
    avatar->nodeo["activatesNodeSolitary"]->addChild(avatar->activateMotiono["grab_right"]);
    avatar->nodeo["activatesNodeSolitary"]->addChild(avatar->activateMotiono["pick_up"]);
    //
    avatar->nodeo["activateNodeTwo"] = avatar->mixer->createNode(NodeType::TWO, "activateNodeTwo");
    avatar->nodeo["activateNodeTwo"]->addChild(avatar->nodeo["landNodeTwo"]);
    avatar->nodeo["activateNodeTwo"]->addChild(avatar->nodeo["activatesNodeSolitary"]);

    // holdAnimations
    avatar->nodeo["holdsNodeSolitary"] = avatar->mixer->createNode(NodeType::SOLITARY, "holdsNodeSolitary");
    avatar->nodeo["holdsNodeSolitary"]->addChild(avatar->holdMotiono["pick_up_idle"]);
    //
    avatar->nodeo["holdNodeFunc"] = avatar->mixer->createNode(NodeType::FUNC, "holdNodeFunc", 0);
    avatar->nodeo["holdNodeFunc"]->addChild(avatar->nodeo["activateNodeTwo"]);
    avatar->nodeo["holdNodeFunc"]->addChild(avatar->nodeo["holdsNodeSolitary"]);

    // set root node ---
    avatar->mixer->setRootNode(avatar->nodeo["holdNodeFunc"]);

    return avatar;
  }
  void Avatar::updateString(char *scratchStack, unsigned int numStrings)
  {
    this->strings.clear();
    unsigned int index = 0;
    for (unsigned int stringIndex = 0; stringIndex < numStrings; stringIndex++)
    {
      unsigned int stringByteLength = (unsigned int)(scratchStack[index++]);
      std::string string = "";
      for (unsigned int charIndex = 0; charIndex < stringByteLength; charIndex++)
      {
        string += scratchStack[index++];
      }
      this->strings.push_back(string);
      // this->strings[stringIndex] = string;
      // std::cout << string << std::endl;
    }
  }
  void Avatar::update(float *scratchStack)
  {
    // std::cout << "idleWalkFactor: " << scratchStack[index] << std::endl;

    unsigned int index = 0;

    // strings ----
    std::string defaultSitAnimation = this->strings[index++];
    std::string defaultEmoteAnimation = this->strings[index++];
    std::string defaultDanceAnimation = this->strings[index++];
    std::string defaultHoldAnimation = this->strings[index++];
    std::string defaultActivateAnimation = this->strings[index++];
    // ---
    std::string useAnimation = this->strings[index++];
    std::string useAnimationCombo = this->strings[index++];
    std::string sitAnimation = this->strings[index++];
    std::string emoteAnimation = this->strings[index++];
    std::string danceAnimation = this->strings[index++];
    std::string holdAnimation = this->strings[index++];
    std::string activateAnimation = this->strings[index++];
    std::string hurtAnimation = this->strings[index++];

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
    this->motiono["walkForward"]->setWeight(forwardFactor);
    this->motiono["walkBackward"]->setWeight(backwardFactor);
    this->motiono["walkLeft"]->setWeight(mirrorLeftFactorReverse);
    this->motiono["walkLeftMirror"]->setWeight(mirrorLeftFactor);
    this->motiono["walkRight"]->setWeight(mirrorRightFactorReverse);
    this->motiono["walkRightMirror"]->setWeight(mirrorRightFactor);

    this->motiono["runForward"]->setWeight(forwardFactor);
    this->motiono["runBackward"]->setWeight(backwardFactor);
    this->motiono["runLeft"]->setWeight(mirrorLeftFactorReverse);
    this->motiono["runLeftMirror"]->setWeight(mirrorLeftFactor);
    this->motiono["runRight"]->setWeight(mirrorRightFactorReverse);
    this->motiono["runRightMirror"]->setWeight(mirrorRightFactor);

    this->motiono["crouchForward"]->setWeight(forwardFactor);
    this->motiono["crouchBackward"]->setWeight(backwardFactor);
    this->motiono["crouchLeft"]->setWeight(mirrorLeftFactorReverse);
    this->motiono["crouchLeftMirror"]->setWeight(mirrorLeftFactor);
    this->motiono["crouchRight"]->setWeight(mirrorRightFactorReverse);
    this->motiono["crouchRightMirror"]->setWeight(mirrorRightFactor);

    this->motiono["bowForward"]->setWeight(forwardFactor);
    this->motiono["bowBackward"]->setWeight(backwardFactor);
    this->motiono["bowLeft"]->setWeight(mirrorLeftFactorReverse);
    this->motiono["bowLeftMirror"]->setWeight(mirrorLeftFactor);
    this->motiono["bowRight"]->setWeight(mirrorRightFactorReverse);
    this->motiono["bowRightMirror"]->setWeight(mirrorRightFactor);

    this->nodeo["_8DirectionsWalkRunNodeTwo"]->setFactor(walkRunFactor);
    this->nodeo["idle8DWalkRunNodeTwo"]->setFactor(idleWalkFactor);
    this->nodeo["idle8DCrouchNodeTwo"]->setFactor(idleWalkFactor);
    this->nodeo["defaultNodeTwo"]->setFactor(crouchFactor);
    this->nodeo["idle8DBowNodeTwo"]->setFactor(idleWalkFactor);

    this->nodeo["flyForwardNodeTwo"]->setWeight(forwardFactor);
    this->motiono["flyDodgeBackward"]->setWeight(backwardFactor);
    this->motiono["flyDodgeLeft"]->setWeight(leftFactor);
    this->motiono["flyDodgeRight"]->setWeight(rightFactor);

    this->nodeo["idle8DFlyNodeTwo"]->setFactor(walkRunFactor);
    this->nodeo["flyForwardNodeTwo"]->setFactor(flyDashFactor);

    this->nodeo["holdNodeFunc"]->setArg(holdFactor);
    this->nodeo["emoteNodeFunc"]->setArg(idleWalkFactor);

    // action end events ---
    if (landEnd) {
      if (!landWithMoving) {
        this->nodeo["landNodeTwo"]->crossFadeTwo(0.05, 0);
      } else {
        this->nodeo["landNodeTwo"]->crossFadeTwo(0.15, 0);
      }
    }
    
    if (fallLoopEnd) {
      this->nodeo["fallLoopNodeTwo"]->crossFadeTwo(0.2, 0);
    }
    
    if (flyEnd) {
      this->nodeo["groundFlyNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (jumpEnd) {
      this->nodeo["jumpNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (doubleJumpEnd) {
      this->nodeo["doubleJumpNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (narutoRunEnd) {
      this->nodeo["narutoRunNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (activateEnd) {
      this->nodeo["activateNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (useEnd) {
      this->nodeo["useNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (useComboEnd) {
      this->nodeo["useCombosNodeSolitary"]->crossFadeSolitary(0.2, this->nodeo["useNodeTwo"]);
    }

    if (useEnvelopeEnd) {
      this->bowMotiono["bowLoose"]->play();
      this->nodeo["bowDrawLooseNodoeTwo"]->setFactor(1);
      this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->crossFadeTwo(0.2, 1);
    }

    if (sitEnd) {
      this->nodeo["sitNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (emoteEnd) {
      this->nodeo["emoteNodeFunc"]->crossFadeTwo(0.2, 0);
    }

    if (hurtEnd) {
      this->nodeo["hurtNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (danceEnd) {
      this->nodeo["danceNodeTwo"]->crossFadeTwo(0.2, 0);
    }

    if (holdEnd) {
      this->nodeo["holdNodeFunc"]->crossFadeTwo(0.2, 0);
    }

    // action start events ---
    if (landStart) {
      // std::cout << "landWithMoving: " << landWithMoving << std::endl;
      // std::cout << "landWithMoving: " << (bool)(landWithMoving) << std::endl;
      if (!landWithMoving) {
        this->motiono["land"]->play();
        this->nodeo["landsNodeSolitary"]->crossFadeSolitary(0, this->motiono["land"]);
        this->nodeo["landNodeTwo"]->crossFadeTwo(0, 1);
      } else {
        this->motiono["land2"]->play();
        this->nodeo["landsNodeSolitary"]->crossFadeSolitary(0, this->motiono["land2"]);
        this->nodeo["landNodeTwo"]->crossFadeTwo(0.1, 1);
      }
    }

    if (fallLoopStart) {
      this->nodeo["fallLoopNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (flyStart) {
      this->nodeo["groundFlyNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (jumpStart) {
      this->motiono["jump"]->play();
      this->nodeo["jumpNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (doubleJumpStart) {
      this->motiono["doubleJump"]->play();
      this->nodeo["doubleJumpNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    if (narutoRunStart) {
      this->nodeo["narutoRunNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // useAnimations // sword
    if (useStart) {
      std::string animationName;
      if (dashAttacking) {
        animationName = "dashAttack";
      } else {
        animationName = useAnimation;
      }
      // std::string animationName = "combo"; // test
      // AnimationNode *useMotion = this->useMotiono[animationName];
      this->useMotiono[animationName]->play();
      this->nodeo["usesNodeSolitary"]->crossFadeSolitary(0, this->useMotiono[animationName]);
      this->nodeo["useNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // useComboAnimations // silsword
    if (useComboStart) {
      std::string animationName;
      if (dashAttacking) {
        animationName = "dashAttack";
      } else {
        // animationName = this->useAnimationCombo[this->useAnimationIndex];
        // animationName = "swordSideSlash"; // test
        animationName = useAnimationCombo;
        // std::cout << "useAnimationCombo: " << useAnimationCombo << std::endl;
      }
      this->useComboMotiono[animationName]->play();
      this->nodeo["useCombosNodeSolitary"]->crossFadeSolitary(0.2, this->useComboMotiono[animationName]);
    }

    // bow
    if (useEnvelopeStart) {
      this->bowMotiono["bowDraw"]->play();
      this->nodeo["bowDrawLooseNodoeTwo"]->setFactor(0);
      this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->setFactor(1);
      this->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // sitAnimations // sit
    if (sitStart) {
      // AnimationNode *sitMotion = this->sitMotiono[sitAnimation == "" ? defaultSitAnimation : sitAnimation];
      std::string animationName = sitAnimation == "" ? defaultSitAnimation : sitAnimation;
      std::cout << "animationName: " << animationName << std::endl;
      this->sitMotiono[animationName]->play();
      this->nodeo["sitsNodeSolitary"]->crossFadeSolitary(0, this->sitMotiono[animationName]);
      this->nodeo["sitNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // emoteAnimations // emote
    if (emoteStart) {
      AnimationNode *emoteMotion = this->emoteMotiono[emoteAnimation == "" ? defaultEmoteAnimation : emoteAnimation];
      emoteMotion->play();
      this->nodeo["emotesNodeSolitary"]->crossFadeSolitary(0, emoteMotion);
      this->nodeo["emoteNodeFunc"]->crossFadeTwo(0.2, 1);
    }

    // hurtAnimations // hurt
    if (hurtStart) {
      AnimationNode *hurtMotion = this->hurtMotiono[hurtAnimation];
      hurtMotion->play();
      this->nodeo["hurtsNodeSolitary"]->crossFadeSolitary(0, hurtMotion);
      this->nodeo["hurtNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // danceAnimations // dance
    if (danceStart) {
      AnimationNode *danceMotion = this->danceMotiono[danceAnimation == "" ? defaultDanceAnimation : danceAnimation];
      danceMotion->play();
      this->nodeo["dancesNodeSolitary"]->crossFadeSolitary(0, danceMotion);
      this->nodeo["danceNodeTwo"]->crossFadeTwo(0.2, 1); // todo: check if node exists
    }

    // holdAnimations // hold
    if (holdStart) {
      AnimationNode *holdMotion = this->holdMotiono[holdAnimation == "" ? defaultHoldAnimation : holdAnimation];
      holdMotion->play();
      this->nodeo["holdsNodeSolitary"]->crossFadeSolitary(0, holdMotion);
      this->nodeo["holdNodeFunc"]->crossFadeTwo(0.2, 1);
    }

    // activateAnimations // activate
    if (activateStart) {
      AnimationNode *activateMotion = this->activateMotiono[activateAnimation == "" ? defaultActivateAnimation : activateAnimation];
      activateMotion->play();
      this->nodeo["activatesNodeSolitary"]->crossFadeSolitary(0, activateMotion);
      this->nodeo["activateNodeTwo"]->crossFadeTwo(0.2, 1);
    }

    // handle finished event ---
    if (this->mixer->finishedFlag)
    {
      std::string finishedMotionName = this->mixer->finishedMotion->name;
      if (useEnvelopeState && finishedMotionName == "bowDraw") {
        this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->crossFadeTwo(0.2, 0);
      }
      if (finishedMotionName == "bowLoose") {
        this->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->crossFadeTwo(0.2, 0);
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
  void AnimationNode::addChild(AnimationNode *child) // todo: check if node exists
  {
    this->children.push_back(child);

    if (this->type == NodeType::SOLITARY)
    {
      if (this->children.size() == 1)
      {
        this->activeNode = child;
        child->weight = 1;
      }
      else
      {
        child->weight = 0;
      }
    }
  }
  void AnimationMixer::setRootNode(AnimationNode *node)
  {
    this->rootNode = node;
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