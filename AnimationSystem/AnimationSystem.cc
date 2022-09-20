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
  // std::map<std::string, Animation *> animationo2; // todo: delete
  // float finishedFlag = 0;
  // std::vector<float> _finishedFlags;
  // float _finishedFlags[100];
  // float finishedAnimationIndex; // todo: use pointer instead of index.
  // float **animationValues = (float **)malloc(53 * sizeof(float)); // ok too
  // Interpolant _interpolant;

  float *localVector = (float *)malloc(3 * sizeof(float));
  // float *localVector2 = (float *)malloc(3 * sizeof(float));

  float *localQuaternion = (float *)malloc(4 * sizeof(float));
  // float *localQuaternion2 = (float *)malloc(4 * sizeof(float));

  // Animation testAnimation = Animation();
  // Animation *testAnimation2 = &testAnimation;
  // Animation **testAnimation3 = &testAnimation2;

  Animation *fallLoopAnimation;
  Animation *floatAnimation;
  Animation *doubleJumpAnimation;
  Animation *jumpAnimation;

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
  float clamp(float value, float minValue, float maxValue)
  {
    return max( minValue, min( maxValue, value ) );
  }
  void _clearXZ(float *dst, bool isPosition)
  {
    if (isPosition)
    {
      dst[0] = 0;
      dst[2] = 0;
    }
  }
  void copyValue(float *dst, float *src, bool isPosition) // todo: delete first value size element.
  {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    if (!isPosition) dst[3] = src[3];
  }
  void interpolateFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t, bool isPosition) // todo: del offsets.
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
  void multiplyQuaternionsFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1) {
    float x0 = src0[ srcOffset0 ];
    float y0 = src0[ srcOffset0 + 1 ];
    float z0 = src0[ srcOffset0 + 2 ];
    float w0 = src0[ srcOffset0 + 3 ];

    float x1 = src1[ srcOffset1 ];
    float y1 = src1[ srcOffset1 + 1 ];
    float z1 = src1[ srcOffset1 + 2 ];
    float w1 = src1[ srcOffset1 + 3 ];

    dst[ dstOffset ] = x0 * w1 + w0 * x1 + y0 * z1 - z0 * y1;
    dst[ dstOffset + 1 ] = y0 * w1 + w0 * y1 + z0 * x1 - x0 * z1;
    dst[ dstOffset + 2 ] = z0 * w1 + w0 * z1 + x0 * y1 - y0 * x1;
    dst[ dstOffset + 3 ] = w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1;
  }
  void invertQuaternionFlat(float *dst, unsigned int dstOffset)
  {
    dst[ dstOffset ] *= -1;
    dst[ dstOffset + 1 ] *= -1;
    dst[ dstOffset + 2 ] *= -1;
  }
  void addVectorsFlat(float *dst, float *src0, float *src1)
  {
    dst[0] = src0[0] + src1[0];
    dst[1] = src0[1] + src1[1];
    dst[2] = src0[2] + src1[2];
  }
  void subVectorsFlat(float *dst, float *src0, float *src1)
  {
    dst[0] = src0[0] - src1[0];
    dst[1] = src0[1] - src1[1];
    dst[2] = src0[2] - src1[2];
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

  Avatar *createAvatar(AnimationMixer *mixer) {
    // avatar->mixer = createAnimationMixer();
    Avatar *avatar = new Avatar();
    avatars.push_back(avatar);
    avatar->mixer = mixer;
    mixer->avatar = avatar; // todo: prevent set `mixer->avatar = avatar` ?

    /*
      {"name": "crouchForward", "type": "motion"}

      {"name": "bowIdle8DDrawLooseNodeOverwrite", "type": "node", "nodeType": "TWO", "funcIndex": 0, "children": [
      ]}
    */
    json tree = json::parse(R"(
      {"name": "defaultNodeTwo", "type": "node", "nodeType": "TWO", "funcIndex": 0, "children": [
        {"name": "idle8DWalkRunNodeTwo", "type": "node", "nodeType": "TWO", "funcIndex": 0, "children": [
          {"name": "idle", "type": "motion"},
          {"name": "_8DirectionsWalkRunNodeTwo", "type": "node", "nodeType": "TWO", "funcIndex": 0, "children": [
            {"name": "_8DirectionsWalkNodeList", "type": "node", "nodeType": "LIST", "funcIndex": 0, "children": [
              {"name": "walkForward", "type": "motion"},
              {"name": "walkBackward", "type": "motion"},
              {"name": "walkLeft", "type": "motion"},
              {"name": "walkRight", "type": "motion"},
              {"name": "walkLeftMirror", "type": "motion"},
              {"name": "walkRightMirror", "type": "motion"}
            ]},
            {"name": "_8DirectionsRunNodeList", "type": "node", "nodeType": "LIST", "funcIndex": 0, "children": [
              {"name": "runForward", "type": "motion"},
              {"name": "runBackward", "type": "motion"},
              {"name": "runLeft", "type": "motion"},
              {"name": "runRight", "type": "motion"},
              {"name": "runLeftMirror", "type": "motion"},
              {"name": "runRightMirror", "type": "motion"}
            ]}
          ]}
        ]},
        {"name": "idle8DCrouchNodeTwo", "type": "node", "nodeType": "TWO", "funcIndex": 0, "children": [
          {"name": "crouchIdle", "type": "motion"},
          {"name": "_8DirectionsCrouchNodeList", "type": "node", "nodeType": "LIST", "funcIndex": 0, "children": [
            {"name": "crouchForward", "type": "motion"},
            {"name": "crouchBackward", "type": "motion"},
            {"name": "crouchLeft", "type": "motion"},
            {"name": "crouchRight", "type": "motion"},
            {"name": "crouchLeftMirror", "type": "motion"},
            {"name": "crouchRightMirror", "type": "motion"}
          ]}
        ]}
      ]}
    )");

    avatar->setAnimations();
    avatar->createMotions();
    // avatar->createNodes();
    // std::cout << "createNodesFromJson ------" << std::endl;
    avatar->createNodesFromJson(tree);
    // std::cout << "root node: " << avatar->mixer->rootNode->name << std::endl;

    //

    avatar->mixer->getNodeTreeData(avatar->mixer->rootNode); // test
    // std::cout << std::endl; //test

    //

    // for (auto const& x : avatar->motiono) // todo: only run once.
    // {
    //   animationo2[x.first] = avatar->motiono[x.first]->animation;
    // }

    return avatar;
  }
  void Avatar::createNodesFromJson(json &jsonNode, AnimationNode *parentNode)
  {
    // std::cout << jsonNode["children"] << std::endl;
    // for (json::iterator it = jsonNode.begin(); it != jsonNode.end(); ++it) {
    //   std::cout << it.key() << " : " << it.value() << "\n";
    // }

    // unsigned int nodeType;
    // switch(jsonNode["nodeType"])
    // {
      
    // }

    if (jsonNode["type"] == "motion")
    {
      AnimationNode *motion = this->motiono[jsonNode["name"]];
      parentNode->addChild(motion);
    }
    else if(jsonNode["type"] == "node")
    {
      // std::cout << "jsonNode index: " << jsonNode["index"] << std::endl;
      unsigned int nodeType;
      // switch(jsonNode["nodeType"])
      // {
      //   case "LIST": nodeType = NodeType::LIST; break;
      //   case "TWO": nodeType = NodeType::TWO; break;
      //   case "SOLITARY": nodeType = NodeType::SOLITARY; break;
      //   case "OVERWRITE": nodeType = NodeType::OVERWRITE; break;
      //   case "FUNC": nodeType = NodeType::FUNC; break;
      // }
      if (jsonNode["nodeType"] == "LIST")
      {
        nodeType = NodeType::LIST;
      }
      else if (jsonNode["nodeType"] == "TWO")
      {
        nodeType = NodeType::TWO;
      }
      else if (jsonNode["nodeType"] == "SOLITARY")
      {
        nodeType = NodeType::SOLITARY;
      }
      else if (jsonNode["nodeType"] == "OVERWRITE")
      {
        nodeType = NodeType::OVERWRITE;
      }
      else if (jsonNode["nodeType"] == "FUNC")
      {
        nodeType = NodeType::FUNC;
      }

      AnimationNode *node;
      // if (jsonNode["index"])
      // {
        node = this->mixer->createNode((NodeType)nodeType, jsonNode["name"], jsonNode["funcIndex"]);
      // }
      // else
      // {
      //   node = this->mixer->createNode((NodeType)nodeType, jsonNode["name"]);
      // }
      // std::cout << "node name: " << node->name << std::endl;
      // std::cout << "node type: " << node->type << std::endl;
      // std::cout << "node index: " << node->index << std::endl;
      this->nodeo[jsonNode["name"]] = node;
      if (parentNode)
      {
        // std::cout << "parent node: " << parentNode->name << std::endl;
        parentNode->addChild(node);
      }
      else
      {
        // std::cout << "set root node: " << node->name << std::endl;
        this->mixer->setRootNode(node);
      }

      // if (jsonNode["children"]) // wrong
      // {
        for (unsigned int i = 0; i < jsonNode["children"].size(); i++)
        {
          // std::cout << jsonNode["children"][i]["name"] << std::endl;
          // this->nodeo[jsonNode["name"]]->addChild();
          json childJsonNode = jsonNode["children"][i];
          // std::cout << childJsonNode["name"] << std::endl;
          this->createNodesFromJson(childJsonNode, node);
        }
      }
    // }
  }
  void Avatar::setAnimations()
  {
    fallLoopAnimation = animationo["falling.fbx"];
    floatAnimation = animationo["treading water.fbx"]; // todo: animations.find(a => a.isFloat);
    doubleJumpAnimation = animationo["jump_double.fbx"];
    jumpAnimation = animationo["jump.fbx"];
  }
  void Avatar::createMotions()
  {
    this->motiono["idle"] = this->mixer->createMotion(animationo["idle.fbx"], "idle"); // todo: don't need `this->`

    this->motiono["walkForward"] = this->mixer->createMotion(animationo["walking.fbx"], "walkForward");
    this->motiono["walkBackward"] = this->mixer->createMotion(animationo["walking backwards.fbx"], "walkBackward");
    this->motiono["walkLeft"] = this->mixer->createMotion(animationo["left strafe walking.fbx"], "walkLeft");
    this->motiono["walkRight"] = this->mixer->createMotion(animationo["right strafe walking.fbx"], "walkRight");
    this->motiono["walkLeftMirror"] = this->mixer->createMotion(animationo["right strafe walking reverse.fbx"], "walkLeftMirror");
    this->motiono["walkRightMirror"] = this->mixer->createMotion(animationo["left strafe walking reverse.fbx"], "walkRightMirror");

    this->motiono["runForward"] = this->mixer->createMotion(animationo["Fast Run.fbx"], "runForward");
    this->motiono["runBackward"] = this->mixer->createMotion(animationo["running backwards.fbx"], "runBackward");
    this->motiono["runLeft"] = this->mixer->createMotion(animationo["left strafe.fbx"], "runLeft");
    this->motiono["runRight"] = this->mixer->createMotion(animationo["right strafe.fbx"], "runRight");
    this->motiono["runLeftMirror"] = this->mixer->createMotion(animationo["right strafe reverse.fbx"], "runLeftMirror");
    this->motiono["runRightMirror"] = this->mixer->createMotion(animationo["left strafe reverse.fbx"], "runRightMirror");

    this->motiono["crouchForward"] = this->mixer->createMotion(animationo["Sneaking Forward.fbx"], "crouchForward");
    this->motiono["crouchBackward"] = this->mixer->createMotion(animationo["Sneaking Forward reverse.fbx"], "crouchBackward");
    this->motiono["crouchLeft"] = this->mixer->createMotion(animationo["Crouched Sneaking Left.fbx"], "crouchLeft");
    this->motiono["crouchRight"] = this->mixer->createMotion(animationo["Crouched Sneaking Right.fbx"], "crouchRight");
    this->motiono["crouchLeftMirror"] = this->mixer->createMotion(animationo["Crouched Sneaking Right reverse.fbx"], "crouchLeftMirror");
    this->motiono["crouchRightMirror"] = this->mixer->createMotion(animationo["Crouched Sneaking Left reverse.fbx"], "crouchRightMirror");

    // this->motiono["bowForward"] = this->mixer->createMotion(animationo["Standing Aim Walk Forward.fbx"], "bowForward");
    // this->motiono["bowBackward"] = this->mixer->createMotion(animationo["Standing Aim Walk Forward reverse.fbx"], "bowBackward");
    // this->motiono["bowLeft"] = this->mixer->createMotion(animationo["Standing Aim Walk Left.fbx"], "bowLeft");
    // this->motiono["bowRight"] = this->mixer->createMotion(animationo["Standing Aim Walk Right.fbx"], "bowRight");
    // this->motiono["bowLeftMirror"] = this->mixer->createMotion(animationo["Standing Aim Walk Right reverse.fbx"], "bowLeftMirror");
    // this->motiono["bowRightMirror"] = this->mixer->createMotion(animationo["Standing Aim Walk Left reverse.fbx"], "bowRightMirror");

    this->motiono["crouchIdle"] = this->mixer->createMotion(animationo["Crouch Idle.fbx"], "crouchIdle");
    this->motiono["fly"] = this->mixer->createMotion(animationo["treading water.fbx"], "fly");
    // this->motiono["flyIdle"] = this->mixer->createMotion(animationo["fly_idle.fbx"], "flyIdle");
    // this->motiono["flyDodgeForward"] = this->mixer->createMotion(animationo["fly_dodge_forward.fbx"], "flyDodgeForward");
    // this->motiono["flyDodgeBackward"] = this->mixer->createMotion(animationo["fly_dodge_backward.fbx"], "flyDodgeBackward");
    // this->motiono["flyDodgeLeft"] = this->mixer->createMotion(animationo["fly_dodge_left.fbx"], "flyDodgeLeft");
    // this->motiono["flyDodgeRight"] = this->mixer->createMotion(animationo["fly_dodge_right.fbx"], "flyDodgeRight");
    // this->motiono["flyDash"] = this->mixer->createMotion(animationo["fly_dash_forward.fbx"], "flyDash");
    this->motiono["narutoRun"] = this->mixer->createMotion(animationo["naruto run.fbx"], "narutoRun");

    this->motiono["jump"] = this->mixer->createMotion(animationo["jump.fbx"], "jump");
    this->motiono["jump"]->setLoop(LoopType::LoopOnce);
    this->motiono["jump"]->stop();

    this->motiono["doubleJump"] = this->mixer->createMotion(animationo["jump_double.fbx"], "doubleJump");
    this->motiono["doubleJump"]->setLoop(LoopType::LoopOnce);
    this->motiono["doubleJump"]->stop();

    this->motiono["fallLoop"] = this->mixer->createMotion(animationo["falling.fbx"], "fallLoop");

    this->motiono["land"] = this->mixer->createMotion(animationo["landing.fbx"], "land");
    this->motiono["land"]->setLoop(LoopType::LoopOnce);
    this->motiono["land"]->stop();
    this->motiono["land"]->setSpeed(0.75);

    this->motiono["land2"] = this->mixer->createMotion(animationo["landing 2.fbx"], "land2");
    this->motiono["land2"]->setLoop(LoopType::LoopOnce);
    this->motiono["land2"]->stop();
    this->motiono["land2"]->setSpeed(1.7);

    // useAnimations
    this->useMotiono["combo"] = this->mixer->createMotion(animationo["One Hand Sword Combo.fbx"], "combo");
    // this->useMotiono["dashAttack"] = this->mixer->createMotion(animationo["sword_dash.fbx"], "dashAttack");
    this->useMotiono["drink"] = this->mixer->createMotion(animationo["drinking.fbx"], "drink");
    this->useMotiono["eat"] = this->mixer->createMotion(animationo["eating.fbx"], "eat");
    this->useMotiono["magic"] = this->mixer->createMotion(animationo["magic cast.fbx"], "magic");
    this->useMotiono["pickUpThrow"] = this->mixer->createMotion(animationo["pick_up_throw.fbx"], "pickUpThrow");
    this->useMotiono["pistol"] = this->mixer->createMotion(animationo["Pistol Aiming Idle.fbx"], "pistol");
    this->useMotiono["rifle"] = this->mixer->createMotion(animationo["Rifle Aiming Idle.fbx"], "rifle");
    this->useMotiono["slash"] = this->mixer->createMotion(animationo["sword and shield slash.fbx"], "slash");
    this->useMotiono["throw"] = this->mixer->createMotion(animationo["pick_up_throw.fbx"], "throw");
    for (auto const& x : this->useMotiono)
    {
      this->motiono[x.first] = this->useMotiono[x.first];
      this->useMotiono[x.first]->setLoop(LoopType::LoopOnce);
      this->useMotiono[x.first]->stop();
    }
    this->useMotiono["combo"]->setSpeed(1.3);
    
    // useComboAnimations
    this->useComboMotiono["swordSideIdle"] = this->mixer->createMotion(animationo["sword_idle_side.fbx"], "swordSideIdle");
    this->useComboMotiono["swordSideSlash"] = this->mixer->createMotion(animationo["sword_side_slash.fbx"], "swordSideSlash");
    this->useComboMotiono["swordSideSlashStep"] = this->mixer->createMotion(animationo["sword_side_slash_step.fbx"], "swordSideSlashStep");
    this->useComboMotiono["swordTopDownSlash"] = this->mixer->createMotion(animationo["sword_topdown_slash.fbx"], "swordTopDownSlash");
    this->useComboMotiono["swordTopDownSlashStep"] = this->mixer->createMotion(animationo["sword_topdown_slash_step.fbx"], "swordTopDownSlashStep");
    // this->useComboMotiono["dashAttack"] = this->mixer->createMotion(animationo["sword_dash.fbx"], "dashAttack");
    for (auto const& x : this->useComboMotiono)
    {
      this->motiono[x.first] = this->useComboMotiono[x.first];
      this->useComboMotiono[x.first]->setLoop(LoopType::LoopOnce);
      this->useComboMotiono[x.first]->stop();
    }

    // bowAnimations
    this->bowMotiono["bowDraw"] = this->mixer->createMotion(animationo["bow draw.fbx"], "bowDraw");
    this->bowMotiono["bowIdle"] = this->mixer->createMotion(animationo["bow idle.fbx"], "bowIdle");
    this->bowMotiono["bowLoose"] = this->mixer->createMotion(animationo["bow loose.fbx"], "bowLoose");
    for (auto const& x : this->bowMotiono)
    {
      this->motiono[x.first] = this->bowMotiono[x.first];
      this->bowMotiono[x.first]->setLoop(LoopType::LoopOnce);
      this->bowMotiono[x.first]->stop();
    }

    // sitAnimations
    this->sitMotiono["chair"] = this->mixer->createMotion(animationo["sitting idle.fbx"], "chair");
    this->sitMotiono["saddle"] = this->mixer->createMotion(animationo["sitting idle.fbx"], "saddle");
    this->sitMotiono["stand"] = this->mixer->createMotion(animationo["Skateboarding.fbx"], "stand");
    for (auto const& x : this->sitMotiono)
    {
      this->motiono[x.first] = this->sitMotiono[x.first];
      this->sitMotiono[x.first]->setLoop(LoopType::LoopOnce);
      this->sitMotiono[x.first]->stop();
    }

    // hurtAnimations
    this->hurtMotiono["pain_back"] = this->mixer->createMotion(animationo["pain_back.fbx"], "pain_back");
    this->hurtMotiono["pain_arch"] = this->mixer->createMotion(animationo["pain_arch.fbx"], "pain_arch");
    for (auto const& x : this->hurtMotiono)
    {
      this->motiono[x.first] = this->hurtMotiono[x.first];
      this->hurtMotiono[x.first]->setLoop(LoopType::LoopOnce);
      this->hurtMotiono[x.first]->stop();
    }

    // emoteAnimations
    this->emoteMotiono["alert"] = this->mixer->createMotion(animationo["alert.fbx"], "alert");
    this->emoteMotiono["alertSoft"] = this->mixer->createMotion(animationo["alert_soft.fbx"], "alertSoft");
    this->emoteMotiono["angry"] = this->mixer->createMotion(animationo["angry.fbx"], "angry");
    this->emoteMotiono["angrySoft"] = this->mixer->createMotion(animationo["angry_soft.fbx"], "angrySoft");
    this->emoteMotiono["embarrassed"] = this->mixer->createMotion(animationo["embarrassed.fbx"], "embarrassed");
    this->emoteMotiono["embarrassedSoft"] = this->mixer->createMotion(animationo["embarrassed_soft.fbx"], "embarrassedSoft");
    this->emoteMotiono["headNod"] = this->mixer->createMotion(animationo["head_nod.fbx"], "headNod");
    this->emoteMotiono["headNodSoft"] = this->mixer->createMotion(animationo["head_nod_single.fbx"], "headNodSoft");
    this->emoteMotiono["headShake"] = this->mixer->createMotion(animationo["head_shake.fbx"], "headShake");
    this->emoteMotiono["headShakeSoft"] = this->mixer->createMotion(animationo["head_shake_single.fbx"], "headShakeSoft");
    this->emoteMotiono["sad"] = this->mixer->createMotion(animationo["sad.fbx"], "sad");
    this->emoteMotiono["sadSoft"] = this->mixer->createMotion(animationo["sad_soft.fbx"], "sadSoft");
    this->emoteMotiono["surprise"] = this->mixer->createMotion(animationo["surprise.fbx"], "surprise");
    this->emoteMotiono["surpriseSoft"] = this->mixer->createMotion(animationo["surprise_soft.fbx"], "surpriseSoft");
    this->emoteMotiono["victory"] = this->mixer->createMotion(animationo["victory.fbx"], "victory");
    this->emoteMotiono["victorySoft"] = this->mixer->createMotion(animationo["victory_soft.fbx"], "victorySoft");
    for (auto const& x : this->emoteMotiono)
    {
      this->motiono[x.first] = this->emoteMotiono[x.first];
      this->emoteMotiono[x.first]->setLoop(LoopType::LoopOnce);
      this->emoteMotiono[x.first]->stop();
    }

    // danceAnimations
    this->danceMotiono["dansu"] = this->mixer->createMotion(animationo["Hip Hop Dancing.fbx"], "dansu");
    this->danceMotiono["powerup"] = this->mixer->createMotion(animationo["powerup.fbx"], "powerup");

    // holdAnimations
    this->holdMotiono["pick_up_idle"] = this->mixer->createMotion(animationo["pick_up_idle.fbx"], "pick_up_idle");
    this->motiono["pick_up_idle"] = this->holdMotiono["pick_up_idle"];

    // activateAnimations
    this->activateMotiono["grab_forward"] = this->mixer->createMotion(animationo["grab_forward.fbx"], "grab_forward");
    this->activateMotiono["grab_down"] = this->mixer->createMotion(animationo["grab_down.fbx"], "grab_down");
    this->activateMotiono["grab_up"] = this->mixer->createMotion(animationo["grab_up.fbx"], "grab_up");
    this->activateMotiono["grab_left"] = this->mixer->createMotion(animationo["grab_left.fbx"], "grab_left");
    this->activateMotiono["grab_right"] = this->mixer->createMotion(animationo["grab_right.fbx"], "grab_right");
    this->activateMotiono["pick_up"] = this->mixer->createMotion(animationo["pick_up.fbx"], "pick_up");
    for (auto const& x : this->activateMotiono)
    {
      this->motiono[x.first] = this->activateMotiono[x.first];
      this->activateMotiono[x.first]->setLoop(LoopType::LoopOnce);
      this->activateMotiono[x.first]->stop();
    }
    this->activateMotiono["grab_forward"]->setSpeed(1.2);
    this->activateMotiono["grab_down"]->setSpeed(1.7);
    this->activateMotiono["grab_up"]->setSpeed(1.2);
    this->activateMotiono["grab_left"]->setSpeed(1.2);
    this->activateMotiono["grab_right"]->setSpeed(1.2);
    this->activateMotiono["pick_up"]->setSpeed(1);

    this->motiono["pickUpZelda"] = this->mixer->createMotion(animationo["pick_up_zelda.fbx"]);
    this->motiono["pickUpIdleZelda"] = this->mixer->createMotion(animationo["pick_up_idle_zelda.fbx"]);

    //
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
    // this->motiono[] = this->mixer->createMotion(animationo[]);
  }
  void Avatar::createNodes()
  {
    // this->nodeo["_8DirectionsWalkNodeList"] = this->mixer->createNode(NodeType::LIST, "_8DirectionsWalkNodeList");
    // this->nodeo["_8DirectionsWalkNodeList"]->addChild(this->motiono["walkForward"]);
    // this->nodeo["_8DirectionsWalkNodeList"]->addChild(this->motiono["walkBackward"]);
    // this->nodeo["_8DirectionsWalkNodeList"]->addChild(this->motiono["walkLeft"]);
    // this->nodeo["_8DirectionsWalkNodeList"]->addChild(this->motiono["walkRight"]);
    // this->nodeo["_8DirectionsWalkNodeList"]->addChild(this->motiono["walkLeftMirror"]);
    // this->nodeo["_8DirectionsWalkNodeList"]->addChild(this->motiono["walkRightMirror"]);

    // this->nodeo["_8DirectionsRunNodeList"] = this->mixer->createNode(NodeType::LIST, "_8DirectionsRunNodeList");
    // this->nodeo["_8DirectionsRunNodeList"]->addChild(this->motiono["runForward"]);
    // this->nodeo["_8DirectionsRunNodeList"]->addChild(this->motiono["runBackward"]);
    // this->nodeo["_8DirectionsRunNodeList"]->addChild(this->motiono["runLeft"]);
    // this->nodeo["_8DirectionsRunNodeList"]->addChild(this->motiono["runRight"]);
    // this->nodeo["_8DirectionsRunNodeList"]->addChild(this->motiono["runLeftMirror"]);
    // this->nodeo["_8DirectionsRunNodeList"]->addChild(this->motiono["runRightMirror"]);

    // this->nodeo["_8DirectionsCrouchNodeList"] = this->mixer->createNode(NodeType::LIST, "_8DirectionsCrouchNodeList");
    // this->nodeo["_8DirectionsCrouchNodeList"]->addChild(this->motiono["crouchForward"]);
    // this->nodeo["_8DirectionsCrouchNodeList"]->addChild(this->motiono["crouchBackward"]);
    // this->nodeo["_8DirectionsCrouchNodeList"]->addChild(this->motiono["crouchLeft"]);
    // this->nodeo["_8DirectionsCrouchNodeList"]->addChild(this->motiono["crouchRight"]);
    // this->nodeo["_8DirectionsCrouchNodeList"]->addChild(this->motiono["crouchLeftMirror"]);
    // this->nodeo["_8DirectionsCrouchNodeList"]->addChild(this->motiono["crouchRightMirror"]);

    // this->nodeo["_8DirectionsBowNodeList"] = this->mixer->createNode(NodeType::LIST, "_8DirectionsBowNodeList");
    // this->nodeo["_8DirectionsBowNodeList"]->addChild(this->motiono["bowForward"]);
    // this->nodeo["_8DirectionsBowNodeList"]->addChild(this->motiono["bowBackward"]);
    // this->nodeo["_8DirectionsBowNodeList"]->addChild(this->motiono["bowLeft"]);
    // this->nodeo["_8DirectionsBowNodeList"]->addChild(this->motiono["bowRight"]);
    // this->nodeo["_8DirectionsBowNodeList"]->addChild(this->motiono["bowLeftMirror"]);
    // this->nodeo["_8DirectionsBowNodeList"]->addChild(this->motiono["bowRightMirror"]);

    // this->nodeo["_8DirectionsWalkRunNodeTwo"] = this->mixer->createNode(NodeType::TWO, "_8DirectionsWalkRunNodeTwo");
    // this->nodeo["_8DirectionsWalkRunNodeTwo"]->addChild(this->nodeo["_8DirectionsWalkNodeList"]);
    // this->nodeo["_8DirectionsWalkRunNodeTwo"]->addChild(this->nodeo["_8DirectionsRunNodeList"]);

    // this->nodeo["idle8DWalkRunNodeTwo"] = this->mixer->createNode(NodeType::TWO, "idle8DWalkRunNodeTwo");
    // this->nodeo["idle8DWalkRunNodeTwo"]->addChild(this->motiono["idle"]);
    // this->nodeo["idle8DWalkRunNodeTwo"]->addChild(this->nodeo["_8DirectionsWalkRunNodeTwo"]);

    // this->nodeo["idle8DCrouchNodeTwo"] = this->mixer->createNode(NodeType::TWO, "idle8DCrouchNodeTwo");
    // this->nodeo["idle8DCrouchNodeTwo"]->addChild(this->motiono["crouchIdle"]);
    // this->nodeo["idle8DCrouchNodeTwo"]->addChild(this->nodeo["_8DirectionsCrouchNodeList"]);

    // this->nodeo["flyForwardNodeTwo"] = this->mixer->createNode(NodeType::TWO, "flyForwardNodeTwo");
    // this->nodeo["flyForwardNodeTwo"]->addChild(this->motiono["flyDodgeForward"]);
    // this->nodeo["flyForwardNodeTwo"]->addChild(this->motiono["flyDash"]);

    // this->nodeo["_8DirectionsFlyNodeList"] = this->mixer->createNode(NodeType::LIST, "_8DirectionsFlyNodeList");
    // this->nodeo["_8DirectionsFlyNodeList"]->addChild(this->nodeo["flyForwardNodeTwo"]);
    // this->nodeo["_8DirectionsFlyNodeList"]->addChild(this->motiono["flyDodgeBackward"]);
    // this->nodeo["_8DirectionsFlyNodeList"]->addChild(this->motiono["flyDodgeLeft"]);
    // this->nodeo["_8DirectionsFlyNodeList"]->addChild(this->motiono["flyDodgeRight"]);

    // this->nodeo["idle8DFlyNodeTwo"] = this->mixer->createNode(NodeType::TWO, "idle8DFlyNodeTwo");
    // this->nodeo["idle8DFlyNodeTwo"]->addChild(this->motiono["flyIdle"]);
    // this->nodeo["idle8DFlyNodeTwo"]->addChild(this->nodeo["_8DirectionsFlyNodeList"]);

    // this->nodeo["idle8DBowNodeTwo"] = this->mixer->createNode(NodeType::TWO, "idle8DBowNodeTwo");
    // this->nodeo["idle8DBowNodeTwo"]->addChild(this->bowMotiono["bowIdle"]);
    // this->nodeo["idle8DBowNodeTwo"]->addChild(this->nodeo["_8DirectionsBowNodeList"]);

    // this->nodeo["bowDrawLooseNodoeTwo"] = this->mixer->createNode(NodeType::TWO, "bowDrawLooseNodoeTwo");
    // this->nodeo["bowDrawLooseNodoeTwo"]->addChild(this->bowMotiono["bowDraw"]);
    // this->nodeo["bowDrawLooseNodoeTwo"]->addChild(this->bowMotiono["bowLoose"]);

    // this->nodeo["bowIdle8DDrawLooseNodeOverwrite"] = this->mixer->createNode(NodeType::OVERWRITE, "bowIdle8DDrawLooseNodeOverwrite");
    // this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->addChild(this->nodeo["idle8DBowNodeTwo"]);
    // this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->addChild(this->nodeo["bowDrawLooseNodoeTwo"]);

    // this->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"] = this->mixer->createNode(NodeType::TWO, "idle8DWalkRun_BowIdle8DDrawLooseNodeTwo");
    // this->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->addChild(this->nodeo["idle8DWalkRunNodeTwo"]);
    // this->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->addChild(this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]);

    // this->nodeo["defaultNodeTwo"] = this->mixer->createNode(NodeType::TWO, "defaultNodeTwo");
    // this->nodeo["defaultNodeTwo"]->addChild(this->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]);
    // this->nodeo["defaultNodeTwo"]->addChild(this->nodeo["idle8DCrouchNodeTwo"]);

    // hurtAnimations
    // this->nodeo["hurtsNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "hurtsNodeSolitary");
    // this->nodeo["hurtsNodeSolitary"]->addChild(this->hurtMotiono["pain_back"]);
    // this->nodeo["hurtsNodeSolitary"]->addChild(this->hurtMotiono["pain_arch"]);
    //
    // this->nodeo["hurtNodeTwo"] = this->mixer->createNode(NodeType::TWO, "hurtNodeTwo");
    // this->nodeo["hurtNodeTwo"]->addChild(this->nodeo["defaultNodeTwo"]);
    // this->nodeo["hurtNodeTwo"]->addChild(this->nodeo["hurtsNodeSolitary"]);

    // useAnimations
    // this->nodeo["usesNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "usesNodeSolitary");
    // https://stackoverflow.com/questions/26281979/c-loop-through-map
    // for (auto const& x : this->useMotiono)
    // {
    //   // std::cout << "useMotiono: " << this->useMotiono[x.first]->name << std::endl;
    //   // this->nodeo["usesNodeSolitary"]->addChild(this->useMotiono[x.first]);
    // }
    //
    // this->nodeo["useNodeTwo"] = this->mixer->createNode(NodeType::TWO, "useNodeTwo");
    // this->nodeo["useNodeTwo"]->addChild(this->nodeo["hurtNodeTwo"]);
    // this->nodeo["useNodeTwo"]->addChild(this->nodeo["usesNodeSolitary"]);

    // useComboAnimations
    // this->nodeo["useCombosNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "useCombosNodeSolitary");
    //
    // this->nodeo["useCombosNodeSolitary"]->addChild(this->nodeo["useNodeTwo"]);
    // //
    // for (auto const& x : this->useComboMotiono)
    // {
    //   this->nodeo["useCombosNodeSolitary"]->addChild(this->useComboMotiono[x.first]);
    // }

    // // emoteAnimations
    // this->nodeo["emotesNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "emotesNodeSolitary");
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["alert"]); // todo: for loop
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["alertSoft"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["angry"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["angrySoft"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["embarrassed"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["embarrassedSoft"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["headNod"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["headNodSoft"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["headShake"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["headShakeSoft"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["sad"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["sadSoft"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["surprise"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["surpriseSoft"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["victory"]);
    // this->nodeo["emotesNodeSolitary"]->addChild(this->emoteMotiono["victorySoft"]);
    // //
    // this->nodeo["emoteNodeFunc"] = this->mixer->createNode(NodeType::FUNC, "emoteNodeFunc", 1);
    // this->nodeo["emoteNodeFunc"]->addChild(this->nodeo["useCombosNodeSolitary"]);
    // this->nodeo["emoteNodeFunc"]->addChild(this->nodeo["emotesNodeSolitary"]);

    // danceAnimations
    // this->nodeo["dancesNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "dancesNodeSolitary");
    // this->nodeo["dancesNodeSolitary"]->addChild(this->danceMotiono["dansu"]);
    // this->nodeo["dancesNodeSolitary"]->addChild(this->danceMotiono["powerup"]);
    //
    // this->nodeo["danceNodeTwo"] = this->mixer->createNode(NodeType::TWO, "danceNodeTwo");
    // this->nodeo["danceNodeTwo"]->addChild(this->nodeo["emoteNodeFunc"]);
    // this->nodeo["danceNodeTwo"]->addChild(this->nodeo["dancesNodeSolitary"]);

    // this->nodeo["narutoRunNodeTwo"] = this->mixer->createNode(NodeType::TWO, "narutoRunNodeTwo");
    // this->nodeo["narutoRunNodeTwo"]->addChild(this->nodeo["danceNodeTwo"]);
    // this->nodeo["narutoRunNodeTwo"]->addChild(this->motiono["narutoRun"]);

    // sitAnimations
    // this->nodeo["sitsNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "sitsNodeSolitary");
    // this->nodeo["sitsNodeSolitary"]->addChild(this->sitMotiono["chair"]);
    // this->nodeo["sitsNodeSolitary"]->addChild(this->sitMotiono["saddle"]);
    // this->nodeo["sitsNodeSolitary"]->addChild(this->sitMotiono["stand"]);
    //
    // this->nodeo["sitNodeTwo"] = this->mixer->createNode(NodeType::TWO, "sitNodeTwo");
    // this->nodeo["sitNodeTwo"]->addChild(this->nodeo["narutoRunNodeTwo"]);
    // this->nodeo["sitNodeTwo"]->addChild(this->nodeo["sitsNodeSolitary"]);

    // this->nodeo["jumpNodeTwo"] = this->mixer->createNode(NodeType::TWO, "jumpNodeTwo");
    // this->nodeo["jumpNodeTwo"]->addChild(this->nodeo["sitNodeTwo"]);
    // this->nodeo["jumpNodeTwo"]->addChild(this->motiono["jump"]);

    // this->nodeo["doubleJumpNodeTwo"] = this->mixer->createNode(NodeType::TWO, "doubleJumpNodeTwo");
    // this->nodeo["doubleJumpNodeTwo"]->addChild(this->nodeo["jumpNodeTwo"]);
    // this->nodeo["doubleJumpNodeTwo"]->addChild(this->motiono["doubleJump"]);

    // this->nodeo["groundFlyNodeTwo"] = this->mixer->createNode(NodeType::TWO, "groundFlyNodeTwo");
    // this->nodeo["groundFlyNodeTwo"]->addChild(this->nodeo["doubleJumpNodeTwo"]);
    // this->nodeo["groundFlyNodeTwo"]->addChild(this->nodeo["idle8DFlyNodeTwo"]);

    // this->nodeo["fallLoopNodeTwo"] = this->mixer->createNode(NodeType::TWO, "fallLoopNodeTwo");
    // this->nodeo["fallLoopNodeTwo"]->addChild(this->nodeo["groundFlyNodeTwo"]);
    // this->nodeo["fallLoopNodeTwo"]->addChild(this->motiono["fallLoop"]);

    // this->nodeo["landsNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "landsNodeSolitary");
    // this->nodeo["landsNodeSolitary"]->addChild(this->motiono["land"]);
    // this->nodeo["landsNodeSolitary"]->addChild(this->motiono["land2"]);
    //
    // this->nodeo["landNodeTwo"] = this->mixer->createNode(NodeType::TWO, "landNodeTwo");
    // this->nodeo["landNodeTwo"]->addChild(this->nodeo["fallLoopNodeTwo"]);
    // this->nodeo["landNodeTwo"]->addChild(this->nodeo["landsNodeSolitary"]);

    // activateAnimations
    // this->nodeo["activatesNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "activatesNodeSolitary");
    // this->nodeo["activatesNodeSolitary"]->addChild(this->activateMotiono["grab_forward"]);
    // this->nodeo["activatesNodeSolitary"]->addChild(this->activateMotiono["grab_down"]);
    // this->nodeo["activatesNodeSolitary"]->addChild(this->activateMotiono["grab_up"]);
    // this->nodeo["activatesNodeSolitary"]->addChild(this->activateMotiono["grab_left"]);
    // this->nodeo["activatesNodeSolitary"]->addChild(this->activateMotiono["grab_right"]);
    // this->nodeo["activatesNodeSolitary"]->addChild(this->activateMotiono["pick_up"]);
    //
    // this->nodeo["activateNodeTwo"] = this->mixer->createNode(NodeType::TWO, "activateNodeTwo");
    // this->nodeo["activateNodeTwo"]->addChild(this->nodeo["landNodeTwo"]);
    // this->nodeo["activateNodeTwo"]->addChild(this->nodeo["activatesNodeSolitary"]);

    // holdAnimations
    // this->nodeo["holdsNodeSolitary"] = this->mixer->createNode(NodeType::SOLITARY, "holdsNodeSolitary");
    // this->nodeo["holdsNodeSolitary"]->addChild(this->holdMotiono["pick_up_idle"]);
    //
    // this->nodeo["holdNodeFunc"] = this->mixer->createNode(NodeType::FUNC, "holdNodeFunc", 0);
    // this->nodeo["holdNodeFunc"]->addChild(this->nodeo["activateNodeTwo"]);
    // this->nodeo["holdNodeFunc"]->addChild(this->nodeo["holdsNodeSolitary"]);

    // set root node ---

    // this->mixer->setRootNode(this->nodeo["holdNodeFunc"]);
    // this->mixer->setRootNode(this->nodeo["_8DirectionsWalkNodeList"]);
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

    // values ---
    float forwardFactor = scratchStack[index++];
    float backwardFactor = scratchStack[index++];
    float leftFactor = scratchStack[index++];
    float rightFactor = scratchStack[index++];
    float mirrorLeftFactorReverse = scratchStack[index++];
    float mirrorLeftFactor = scratchStack[index++];
    float mirrorRightFactorReverse = scratchStack[index++];
    float mirrorRightFactor = scratchStack[index++];

    this->idleWalkFactor = scratchStack[index++];
    this->walkRunFactor = scratchStack[index++];
    this->crouchFactor = scratchStack[index++];
    // float flyDashFactor = scratchStack[index++];

    float holdFactor = scratchStack[index++];

    this->activateTime = scratchStack[index++];

    // action states ---
    this->jumpState = scratchStack[index++];
    this->doubleJumpState = scratchStack[index++];
    bool landState = scratchStack[index++];
    this->flyState = scratchStack[index++];
    bool activateState = scratchStack[index++];
    this->narutoRunState = scratchStack[index++];
    this->sitState = scratchStack[index++];
    this->holdState = scratchStack[index++];
    bool emoteState = scratchStack[index++];
    bool fallLoopState = scratchStack[index++];
    bool hurtState = scratchStack[index++];
    bool danceState = scratchStack[index++];
    bool useEnvelopeState = scratchStack[index++]; // todo: clean this and other unused.
    this->pickUpState = scratchStack[index++];

    // action end events ---
    // float landEnd = scratchStack[index++];
    // float fallLoopEnd = scratchStack[index++];
    // float flyEnd = scratchStack[index++];
    // float jumpEnd = scratchStack[index++];
    // float doubleJumpEnd = scratchStack[index++];
    // float narutoRunEnd = scratchStack[index++];
    // float activateEnd = scratchStack[index++];
    // float useEnd = scratchStack[index++];
    // float useComboEnd = scratchStack[index++];
    // float useEnvelopeEnd = scratchStack[index++];
    // float sitEnd = scratchStack[index++];
    // float emoteEnd = scratchStack[index++];
    // float hurtEnd = scratchStack[index++];
    // float danceEnd = scratchStack[index++];
    // float holdEnd = scratchStack[index++];

    // action start events ---
    // float landStart = scratchStack[index++];
    // float fallLoopStart = scratchStack[index++];
    // float flyStart = scratchStack[index++];
    // float jumpStart = scratchStack[index++];
    // float doubleJumpStart = scratchStack[index++];
    // float narutoRunStart = scratchStack[index++];
    // float useStart = scratchStack[index++];
    // float useComboStart = scratchStack[index++];
    // float useEnvelopeStart = scratchStack[index++];
    // float sitStart = scratchStack[index++];
    // float emoteStart = scratchStack[index++];
    // float hurtStart = scratchStack[index++];
    // float danceStart = scratchStack[index++];
    // float holdStart = scratchStack[index++];
    // float activateStart = scratchStack[index++];

    // other ---
    this->landWithMoving = scratchStack[index++];
    float dashAttacking = scratchStack[index++];
    this->landTime = scratchStack[index++];
    this->fallLoopFactor = scratchStack[index++];
    this->fallLoopTime = scratchStack[index++];
    this->flyTime = scratchStack[index++];
    this->doubleJumpTime = scratchStack[index++];
    this->jumpTime = scratchStack[index++];
    this->narutoRunTime = scratchStack[index++];
    this->narutoRunTimeFactor = scratchStack[index++];
    this->danceFactor = scratchStack[index++];
    this->crouchMaxTime = scratchStack[index++];
    this->emoteFactor = scratchStack[index++];
    this->lastEmoteTime = scratchStack[index++];
    this->useTime = scratchStack[index++];
    this->useAnimationEnvelopeLength = scratchStack[index++];
    this->hurtTime = scratchStack[index++];
    this->unuseTime = scratchStack[index++];
    this->aimTime = scratchStack[index++];
    this->aimMaxTime = scratchStack[index++];
    this->pickUpTime = scratchStack[index++];

    // ---------------------------------------------------------------------------------------------------

    index = 0;

    // strings ----
    this->defaultSitAnimation = this->strings[index++]; // todo: defaultSitAnimationName, only rename on wasm side.
    this->defaultEmoteAnimationName = this->strings[index++];
    this->defaultDanceAnimationName = this->strings[index++];
    std::string defaultHoldAnimation = this->strings[index++];
    std::string defaultActivateAnimation = this->strings[index++]; // todo: defaultActivateAnimationName
    this->defaultNarutoRunAnimation = this->strings[index++]; // todo: defaultActivateAnimationName
    // ---
    this->useAnimationName = this->strings[index++];
    this->useAnimationComboName = this->strings[index++];
    this->sitAnimation = this->strings[index++]; // todo: sitAnimationName, only rename on wasm side.
    this->emoteAnimationName = this->strings[index++];
    this->danceAnimationName = this->strings[index++];
    std::string holdAnimation = this->strings[index++];
    this->activateAnimationName = this->strings[index++];
    this->hurtAnimationName = this->strings[index++];
    this->unuseAnimationName = this->strings[index++];
    this->aimAnimationName = this->strings[index++];
    // ---
    this->fallLoopFrom = this->strings[index++];

    useAnimationEnvelopeNames.clear();
    for (unsigned int i = 0; i < useAnimationEnvelopeLength; i++) {
      // std::cout << this->strings[index] << std::endl;
      useAnimationEnvelopeNames.push_back(this->strings[index++]);
    }

    // std::cout << "sitAnimation: " << sitAnimation << std::endl;

    // ---------------------------------------------------------------------------------------------------

    // // set start/end events ---
    // this->jumpStart = false;
    // this->jumpEnd = false;
    // if (jumpState != this->lastJumpState)
    // {
    //   if (jumpState) this->jumpStart = true;
    //   else this->jumpEnd = true;
    // }
    // this->lastJumpState = jumpState;
    
    // this->doubleJumpStart = false;
    // this->doubleJumpEnd = false;
    // if (doubleJumpState != this->lastDoubleJumpState)
    // {
    //   if (doubleJumpState) this->doubleJumpStart = true;
    //   else this->doubleJumpEnd = true;
    // }
    // this->lastDoubleJumpState = doubleJumpState;
    
    // this->landStart = false;
    // this->landEnd = false;
    // if (landState != this->lastLandState)
    // {
    //   if (landState) this->landStart = true;
    //   else this->landEnd = true;
    // }
    // this->lastLandState = landState;
    
    // this->flyStart = false;
    // this->flyEnd = false;
    // if (flyState != this->lastFlyState)
    // {
    //   if (flyState) this->flyStart = true;
    //   else this->flyEnd = true;
    // }
    // this->lastFlyState = flyState;
    
    // this->activateStart = false;
    // this->activateEnd = false;
    // if (activateState != this->lastActivateState)
    // {
    //   if (activateState) this->activateStart = true;
    //   else this->activateEnd = true;
    // }
    // this->lastActivateState = activateState;
    
    // this->narutoRunStart = false;
    // this->narutoRunEnd = false;
    // if (narutoRunState != this->lastNarutoRunState)
    // {
    //   if (narutoRunState) this->narutoRunStart = true;
    //   else this->narutoRunEnd = true;
    // }
    // this->lastNarutoRunState = narutoRunState;
    
    // this->sitStart = false;
    // this->sitEnd = false;
    // if (sitState != this->lastSitState)
    // {
    //   if (sitState) this->sitStart = true;
    //   else this->sitEnd = true;
    // }
    // this->lastSitState = sitState;
    
    // this->holdStart = false;
    // this->holdEnd = false;
    // if (holdState != this->lastHoldState)
    // {
    //   if (holdState) this->holdStart = true;
    //   else this->holdEnd = true;
    // }
    // this->lastHoldState = holdState;
    
    // this->emoteStart = false;
    // this->emoteEnd = false;
    // if (emoteState != this->lastEmoteState)
    // {
    //   if (emoteState) this->emoteStart = true;
    //   else this->emoteEnd = true;
    // }
    // this->lastEmoteState = emoteState;
    
    // this->fallLoopStart = false;
    // this->fallLoopEnd = false;
    // if (fallLoopState != this->lastFallLoopState)
    // {
    //   if (fallLoopState) this->fallLoopStart = true;
    //   else this->fallLoopEnd = true;
    // }
    // this->lastFallLoopState = fallLoopState;
    
    // this->danceStart = false;
    // this->danceEnd = false;
    // if (danceState != this->lastDanceState)
    // {
    //   if (danceState) this->danceStart = true;
    //   else this->danceEnd = true;
    // }
    // this->lastDanceState = danceState;
    
    // this->useEnvelopeStart = false;
    // this->useEnvelopeEnd = false;
    // if (useEnvelopeState != this->lastUseEnvelopeState)
    // {
    //   if (useEnvelopeState) this->useEnvelopeStart = true;
    //   else this->useEnvelopeEnd = true;
    // }
    // this->lastUseEnvelopeState = useEnvelopeState;

    //
    
    // // std::cout << "useAnimation: " << useAnimation << std::endl;
    // this->useStart = false;
    // this->useEnd = false;
    // if (useAnimation != this->lastUseAnimation)
    // {
    //   if (useAnimation.length() > 0) this->useStart = true;
    //   else this->useEnd = true;
    // }
    // this->lastUseAnimation = useAnimation;

    //
    
    // // std::cout << "useAnimationComboName: " << useAnimationComboName << std::endl;
    // this->useComboStart = false;
    // this->useComboEnd = false;
    // if (useAnimationComboName != this->lastUseAnimationComboName)
    // {
    //   if (useAnimationComboName.length() > 0) this->useComboStart = true;
    //   else this->useComboEnd = true;
    // }
    // this->lastUseAnimationComboName = useAnimationComboName;

    // std::cout
    //   << "useStart: " << useStart << ", "
    //   << "useEnd: " << useEnd << ", "
    //   << "useComboStart: " << useComboStart << ", "
    //   << "useComboEnd: " << useComboEnd << ", "
    //   << std::endl;

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

    // this->motiono["bowForward"]->setWeight(forwardFactor);
    // this->motiono["bowBackward"]->setWeight(backwardFactor);
    // this->motiono["bowLeft"]->setWeight(mirrorLeftFactorReverse);
    // this->motiono["bowLeftMirror"]->setWeight(mirrorLeftFactor);
    // this->motiono["bowRight"]->setWeight(mirrorRightFactorReverse);
    // this->motiono["bowRightMirror"]->setWeight(mirrorRightFactor);

    this->nodeo["_8DirectionsWalkRunNodeTwo"]->setFactor(this->walkRunFactor);
    this->nodeo["idle8DWalkRunNodeTwo"]->setFactor(this->idleWalkFactor);
    this->nodeo["idle8DCrouchNodeTwo"]->setFactor(this->idleWalkFactor);
    this->nodeo["defaultNodeTwo"]->setFactor(this->crouchFactor);
    // this->nodeo["idle8DBowNodeTwo"]->setFactor(this->idleWalkFactor);

    // this->nodeo["flyForwardNodeTwo"]->setWeight(forwardFactor);
    // this->motiono["flyDodgeBackward"]->setWeight(backwardFactor);
    // this->motiono["flyDodgeLeft"]->setWeight(leftFactor);
    // this->motiono["flyDodgeRight"]->setWeight(rightFactor);

    // this->nodeo["idle8DFlyNodeTwo"]->setFactor(walkRunFactor);
    // this->nodeo["flyForwardNodeTwo"]->setFactor(flyDashFactor);

    // this->nodeo["holdNodeFunc"]->setArg(holdFactor);
    // this->nodeo["emoteNodeFunc"]->setArg(this->idleWalkFactor); // todo: del

    // // action end events ---
    // if (this->landEnd) {
    //   if (!landWithMoving) {
    //     this->nodeo["landNodeTwo"]->crossFadeTwo(0.05, 0);
    //   } else {
    //     this->nodeo["landNodeTwo"]->crossFadeTwo(0.15, 0);
    //   }
    // }
    
    // if (this->fallLoopEnd) {
    //   this->nodeo["fallLoopNodeTwo"]->crossFadeTwo(0.2, 0);
    // }
    
    // if (this->flyEnd) {
    //   this->nodeo["groundFlyNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->jumpEnd) {
    //   this->nodeo["jumpNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->doubleJumpEnd) {
    //   this->nodeo["doubleJumpNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->narutoRunEnd) {
    //   this->nodeo["narutoRunNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->activateEnd) {
    //   this->nodeo["activateNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->useEnd) {
    //   this->nodeo["useNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->useComboEnd) {
    //   this->nodeo["useCombosNodeSolitary"]->crossFadeSolitary(0.2, this->nodeo["useNodeTwo"]);
    // }

    // if (this->useEnvelopeEnd) {
    //   this->bowMotiono["bowLoose"]->play();
    //   this->nodeo["bowDrawLooseNodoeTwo"]->setFactor(1);
    //   this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->crossFadeTwo(0.2, 1);
    // }

    // if (this->sitEnd) {
    //   this->nodeo["sitNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->emoteEnd) {
    //   this->nodeo["emoteNodeFunc"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->hurtEnd) {
    //   this->nodeo["hurtNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->danceEnd) {
    //   this->nodeo["danceNodeTwo"]->crossFadeTwo(0.2, 0);
    // }

    // if (this->holdEnd) {
    //   this->nodeo["holdNodeFunc"]->crossFadeTwo(0.2, 0);
    // }

    // // action start events ---
    // if (this->landStart) {
    //   // std::cout << "landWithMoving: " << landWithMoving << std::endl;
    //   // std::cout << "landWithMoving: " << (bool)(landWithMoving) << std::endl;
    //   if (!landWithMoving) {
    //     this->motiono["land"]->play();
    //     this->nodeo["landsNodeSolitary"]->crossFadeSolitary(0, this->motiono["land"]);
    //     this->nodeo["landNodeTwo"]->crossFadeTwo(0, 1);
    //   } else {
    //     this->motiono["land2"]->play();
    //     this->nodeo["landsNodeSolitary"]->crossFadeSolitary(0, this->motiono["land2"]);
    //     this->nodeo["landNodeTwo"]->crossFadeTwo(0.1, 1);
    //   }
    // }

    // if (this->fallLoopStart) {
    //   this->nodeo["fallLoopNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // if (this->flyStart) {
    //   this->nodeo["groundFlyNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // if (this->jumpStart) {
    //   this->motiono["jump"]->play();
    //   this->nodeo["jumpNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // if (this->doubleJumpStart) {
    //   this->motiono["doubleJump"]->play();
    //   this->nodeo["doubleJumpNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // if (this->narutoRunStart) {
    //   this->nodeo["narutoRunNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // // useAnimations // sword
    // if (this->useStart) {
    //   std::string animationName;
    //   if (dashAttacking) {
    //     animationName = "dashAttack";
    //   } else {
    //     animationName = useAnimation;
    //   }
    //   // std::string animationName = "combo"; // test
    //   // AnimationNode *useMotion = this->useMotiono[animationName];
    //   this->useMotiono[animationName]->play();
    //   this->nodeo["usesNodeSolitary"]->crossFadeSolitary(0, this->useMotiono[animationName]);
    //   this->nodeo["useNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // // useComboAnimations // silsword
    // if (this->useComboStart) {
    //   std::string animationName;
    //   if (dashAttacking) {
    //     animationName = "dashAttack";
    //   } else {
    //     // animationName = this->useAnimationComboName[this->useAnimationIndex];
    //     // animationName = "swordSideSlash"; // test
    //     animationName = useAnimationComboName;
    //     // std::cout << "useAnimationComboName: " << useAnimationComboName << std::endl;
    //   }
    //   this->useComboMotiono[animationName]->play();
    //   this->nodeo["useCombosNodeSolitary"]->crossFadeSolitary(0.2, this->useComboMotiono[animationName]);
    // }

    // // bow
    // if (this->useEnvelopeStart) {
    //   this->bowMotiono["bowDraw"]->play();
    //   this->nodeo["bowDrawLooseNodoeTwo"]->setFactor(0);
    //   this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->setFactor(1);
    //   this->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // // sitAnimations // sit
    // if (this->sitStart) {
    //   // AnimationNode *sitMotion = this->sitMotiono[sitAnimation == "" ? defaultSitAnimation : sitAnimation];
    //   std::string animationName = sitAnimation == "" ? defaultSitAnimation : sitAnimation;
    //   // std::cout << "animationName: " << animationName << std::endl;
    //   this->sitMotiono[animationName]->play();
    //   this->nodeo["sitsNodeSolitary"]->crossFadeSolitary(0, this->sitMotiono[animationName]);
    //   this->nodeo["sitNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // // emoteAnimations // emote
    // if (this->emoteStart) {
    //   AnimationNode *emoteMotion = this->emoteMotiono[emoteAnimation == "" ? defaultEmoteAnimation : emoteAnimation];
    //   emoteMotion->play();
    //   this->nodeo["emotesNodeSolitary"]->crossFadeSolitary(0, emoteMotion);
    //   this->nodeo["emoteNodeFunc"]->crossFadeTwo(0.2, 1);
    // }

    // // hurtAnimations // hurt
    // if (this->hurtStart) {
    //   AnimationNode *hurtMotion = this->hurtMotiono[hurtAnimation];
    //   hurtMotion->play();
    //   this->nodeo["hurtsNodeSolitary"]->crossFadeSolitary(0, hurtMotion);
    //   this->nodeo["hurtNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // // danceAnimations // dance
    // if (this->danceStart) {
    //   AnimationNode *danceMotion = this->danceMotiono[danceAnimation == "" ? defaultDanceAnimation : danceAnimation];
    //   danceMotion->play();
    //   this->nodeo["dancesNodeSolitary"]->crossFadeSolitary(0, danceMotion);
    //   this->nodeo["danceNodeTwo"]->crossFadeTwo(0.2, 1); // todo: check if node exists
    // }

    // // holdAnimations // hold
    // if (this->holdStart) {
    //   AnimationNode *holdMotion = this->holdMotiono[holdAnimation == "" ? defaultHoldAnimation : holdAnimation];
    //   holdMotion->play();
    //   this->nodeo["holdsNodeSolitary"]->crossFadeSolitary(0, holdMotion);
    //   this->nodeo["holdNodeFunc"]->crossFadeTwo(0.2, 1);
    // }

    // // activateAnimations // activate
    // if (this->activateStart) {
    //   AnimationNode *activateMotion = this->activateMotiono[activateAnimation == "" ? defaultActivateAnimation : activateAnimation];
    //   activateMotion->play();
    //   this->nodeo["activatesNodeSolitary"]->crossFadeSolitary(0, activateMotion);
    //   this->nodeo["activateNodeTwo"]->crossFadeTwo(0.2, 1);
    // }

    // // handle finished event ---
    // if (this->mixer->finishedFlag)
    // {
    //   std::string finishedMotionName = this->mixer->finishedMotion->name;
    //   if (useEnvelopeState && finishedMotionName == "bowDraw") {
    //     this->nodeo["bowIdle8DDrawLooseNodeOverwrite"]->crossFadeTwo(0.2, 0);
    //   }
    //   if (finishedMotionName == "bowLoose") {
    //     this->nodeo["idle8DWalkRun_BowIdle8DDrawLooseNodeTwo"]->crossFadeTwo(0.2, 0);
    //   }
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
  void AnimationMixer::plotNodeTree(AnimationNode *node, unsigned int indent)
  {
    for (unsigned int i = 0; i < indent; i++)
    {
      std::cout << " ";
    }
    std::cout << node->name << std::endl;
    for (unsigned int i = 0; i < node->children.size(); i++)
    {
      this->plotNodeTree(node->children[i], indent + 1);
    }
  }
  void AnimationMixer::getNodeTreeData(AnimationNode *node)
  {
    std::cout << "{\"name\":\"" << node->name << "\",\"children\":[";
    for (unsigned int i = 0; i < node->children.size(); i++)
    {
      this->getNodeTreeData(node->children[i]);
    }
    std::cout << "]},";
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
    // interpolant.resultBuffer = (float *)malloc((1 + valueSize) * sizeof(float)); // 1 len for valueSize self ( 3 or 4 ). 3 len or 4 len for values.
    interpolant.resultBuffer = (float *)malloc(valueSize * sizeof(float));
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
      interpolant.resultBuffer[0] = interpolant.sampleValues[0];
      interpolant.resultBuffer[1] = interpolant.sampleValues[1];
      interpolant.resultBuffer[2] = interpolant.sampleValues[2];
      if (interpolant.valueSize == 4)
      {
        interpolant.resultBuffer[3] = interpolant.sampleValues[3];
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
        interpolant.resultBuffer[0] = interpolant.sampleValues[0];
        interpolant.resultBuffer[1] = interpolant.sampleValues[1];
        interpolant.resultBuffer[2] = interpolant.sampleValues[2];
        if (interpolant.valueSize == 4)
        {
          interpolant.resultBuffer[3] = interpolant.sampleValues[3];
        }
      }
      else if (index > interpolant.numParameterPositions - 1)
      { // Handle situation that, t > max parameterPosition.
        unsigned int maxIndex = interpolant.numParameterPositions - 1;
        interpolant.resultBuffer[0] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 0];
        interpolant.resultBuffer[1] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 1];
        interpolant.resultBuffer[2] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 2];
        if (interpolant.valueSize == 4)
        {
          interpolant.resultBuffer[3] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 3];
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

        if (interpolant.valueSize == 3) // todo: use interpolateFlat
        {
          lerpFlat(
              interpolant.resultBuffer, 0,
              interpolant.sampleValues, index0 * interpolant.valueSize,
              interpolant.sampleValues, index1 * interpolant.valueSize,
              f);
        }
        else
        {
          slerpFlat(
              interpolant.resultBuffer, 0,
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

  void _blendPickUp(AnimationMapping &spec, Avatar *avatar) {
    Animation *pickUpAnimation = avatar->motiono["pickUpZelda"]->animation;
    Animation *pickUpIdleAnimation = avatar->motiono["pickUpIdleZelda"]->animation;

    float t2 = avatar->pickUpTime / 1000;
    if (t2 < pickUpAnimation->duration) {
      float *v2 = evaluateInterpolant(pickUpAnimation, spec.index, t2);
      copyValue(spec.dst, v2, spec.isPosition);
    } else {
      float t3 = fmod((t2 - pickUpAnimation->duration), pickUpIdleAnimation->duration);
      float *v2 = evaluateInterpolant(pickUpIdleAnimation, spec.index, t3);
      copyValue(spec.dst, v2, spec.isPosition);
    }
  }
  void _blendHold(AnimationMapping &spec, Avatar *avatar) {
    // _handleDefault(spec);

    Animation *holdAnimation = avatar->motiono["pick_up_idle"]->animation;
    float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
    float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);

    if (spec.isTop) {
      if (spec.index == BoneName::Left_arm || spec.index == BoneName::Right_arm) {
        copyValue(spec.dst, v2, spec.isPosition);
      } else {
        if (spec.isArm) {
          float f = avatar->walkRunFactor * 0.7 + avatar->crouchFactor * (1 - avatar->idleWalkFactor) * 0.5;
          interpolateFlat(spec.dst, 0, spec.dst, 0, identityQuaternion, 0, f, spec.isPosition);
          multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
        } else {
          multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
        }
      }
    }
  }
  void _blendAim(AnimationMapping &spec, Avatar *avatar) {
    Animation *aimAnimation = avatar->motiono[avatar->aimAnimationName]->animation;
    // std::cout << "aimAnimationName: " << avatar->aimAnimationName << std::endl;
    // _handleDefault(spec); // todo: restore _handleDefault() instead of blendList.
    float t2 = fmod((avatar->aimTime / avatar->aimMaxTime), aimAnimation->duration);
    if (!spec.isPosition) {
      if (aimAnimation) {
        float *v2 = evaluateInterpolant(aimAnimation, spec.index, t2);

        Animation *idleAnimation = animationo["idle.fbx"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      }
    } else {
      float *v2 = evaluateInterpolant(aimAnimation, spec.index, t2);

      Animation *idleAnimation = animationo["idle.fbx"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

      subVectorsFlat(spec.dst, spec.dst, v3);
      addVectorsFlat(spec.dst, spec.dst, v2);
    }
  }
  void _blendUnuse(AnimationMapping &spec, Avatar *avatar) {
    if (spec.isPosition) avatar->testString += "_blendUnuse, "; // test
    float unuseTimeS = avatar->unuseTime / 1000;
    std::string unuseAnimationName = avatar->unuseAnimationName; // todo: performance: use avatar->unuseAnimationName directly.
    Animation *unuseAnimation = avatar->motiono[unuseAnimationName]->animation;
    float t2 = min(unuseTimeS, unuseAnimation->duration);
    float f = min(max(unuseTimeS / unuseAnimation->duration, 0), 1);
    float f2 = std::pow(1 - f, 2);
    // std::cout << " f2: " << f2 << std::endl;

    // copyValue(spec.dst, evaluateInterpolant(unuseAnimation, spec.index, t2), spec.isPosition); // test

    // return;

    if (!spec.isPosition) {
      float *v2 = evaluateInterpolant(unuseAnimation, spec.index, t2);

      Animation *idleAnimation = animationo["idle.fbx"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
        
      copyValue(localQuaternion, spec.dst, spec.isPosition);
      invertQuaternionFlat(v3, 0);
      multiplyQuaternionsFlat(localQuaternion, 0, v3, 0, localQuaternion, 0);
      multiplyQuaternionsFlat(localQuaternion, 0, v2, 0, localQuaternion, 0);

      interpolateFlat(spec.dst, 0, spec.dst, 0, localQuaternion, 0, f2, spec.isPosition);
    } else {
      float *v2 = evaluateInterpolant(unuseAnimation, spec.index, t2);

      Animation *idleAnimation = animationo["idle.fbx"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
      
      copyValue(localVector, spec.dst, spec.isPosition);
      subVectorsFlat(localVector, localVector, v3);
      addVectorsFlat(localVector, localVector, v2);

      interpolateFlat(spec.dst, 0, spec.dst, 0, localVector, 0, f2, spec.isPosition);
    }

    // todo: don't need ?
    // if (f >= 1) {
    //   avatar.useAnimation = ''; // todo: need feedback to js side ?
    // }
  }
  void _blendHurt(AnimationMapping &spec, Avatar *avatar) {
    if (spec.isPosition) avatar->testString += "_blendHurt, "; // test
    Animation *hurtAnimation = avatar->motiono[avatar->hurtAnimationName]->animation;
    float hurtTimeS = avatar->hurtTime / 1000;
    float t2 = min(hurtTimeS, hurtAnimation->duration);
    if (!spec.isPosition) {
      if (hurtAnimation) {
        // const src2 = hurtAnimation.interpolants[k];
        // const v2 = src2.evaluate(t2);
        float *v2 = evaluateInterpolant(hurtAnimation, spec.index, t2);

        Animation *idleAnimation = animationo["idle.fbx"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
        
        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      }
    } else {
      // const src2 = hurtAnimation.interpolants[k];
      // const v2 = src2.evaluate(t2);
      float *v2 = evaluateInterpolant(hurtAnimation, spec.index, t2);

      Animation *idleAnimation = animationo["idle.fbx"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

      subVectorsFlat(spec.dst, spec.dst, v3);
      addVectorsFlat(spec.dst, spec.dst, v2);
    }
  }
  void _blendUse(AnimationMapping &spec, Avatar *avatar)
  {
    if (spec.isPosition) avatar->testString += "_blendUse, "; // test
    // std::cout << "useAnimationName" << std::endl;
    Animation *useAnimation = nullptr;
    float t2;
    float useTimeS = avatar->useTime / 1000;
    if (avatar->useAnimationName != "") {
      useAnimation = avatar->motiono[avatar->useAnimationName]->animation;
      t2 = min(useTimeS, useAnimation->duration);
    } else if(avatar->useAnimationComboName != "") {
      useAnimation = avatar->motiono[avatar->useAnimationComboName]->animation;
      t2 = min(useTimeS, useAnimation->duration);
    } else if (avatar->useAnimationEnvelopeNames.size() > 0) { // todo: why not bowLoose and transition animations in the end ?
        float totalTime = 0;
        for (unsigned int i = 0; i < avatar->useAnimationEnvelopeNames.size() - 1; i++) {
          std::string animationName = avatar->useAnimationEnvelopeNames[i];
          Animation *animation = avatar->motiono[animationName]->animation;
          totalTime += animation->duration;
        }

        if (totalTime > 0) {
          float animationTimeBase = 0;
          for (unsigned int i = 0; i < avatar->useAnimationEnvelopeNames.size() - 1; i++) {
            std::string animationName = avatar->useAnimationEnvelopeNames[i];
            Animation *animation = avatar->motiono[animationName]->animation;
            if (useTimeS < (animationTimeBase + animation->duration)) {
              useAnimation = animation;
              break;
            }
            animationTimeBase += animation->duration;
          }
          if (useAnimation != nullptr) { // first iteration
            t2 = min(useTimeS - animationTimeBase, useAnimation->duration);
          } else { // loop
            std::string secondLastAnimationName = avatar->useAnimationEnvelopeNames[avatar->useAnimationEnvelopeNames.size() - 2];
            useAnimation = avatar->motiono[secondLastAnimationName]->animation;
            t2 = fmod((useTimeS - animationTimeBase), useAnimation->duration);
          }
        }
      }

    if (useAnimation) {
      if (!spec.isPosition) {
        float *v2 = evaluateInterpolant(useAnimation, spec.index, t2);

        Animation *idleAnimation = animationo["idle.fbx"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      } else {
        float *v2 = evaluateInterpolant(useAnimation, spec.index, t2);
        _clearXZ(v2, spec.isPosition);

        Animation *idleAnimation = animationo["idle.fbx"]; // todo: don't always idle.fbx ? Walk Run Crouch ?
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        subVectorsFlat(spec.dst, spec.dst, v3);
        addVectorsFlat(spec.dst, spec.dst, v2);
      }
    }
  }
  void _blendEmote(AnimationMapping &spec, Avatar *avatar)
  {
    if (spec.isPosition) avatar->testString += "_blendEmote, "; // test
    // _handleDefault(spec); // todo: prevent run forever ?

    // const emoteAnimation = emoteAnimations[avatar->emoteAnimation || defaultEmoteAnimation];
    Animation *emoteAnimation = avatar->emoteMotiono[avatar->emoteAnimationName == "" ? avatar->defaultEmoteAnimationName : avatar->emoteAnimationName]->animation;
    // const src2 = emoteAnimation.interpolants[k];
    float emoteTime = AnimationMixer::nowS * 1000 - avatar->lastEmoteTime; // todo: use now.
    // float emoteTime = 0; // test
    float t2 = min(emoteTime / 1000, emoteAnimation->duration);
    // const v2 = src2.evaluate(t2);
    float *v2 = evaluateInterpolant(emoteAnimation, spec.index, t2);
    // if (spec.isPosition) std::cout << " t2: " << t2 << std::endl;

    float emoteFactorS = avatar->emoteFactor / avatar->crouchMaxTime;
    float f = min(max(emoteFactorS, 0), 1);

    if (spec.index == BoneName::Spine || spec.index == BoneName::Chest || spec.index == BoneName::UpperChest || spec.index == BoneName::Neck || spec.index == BoneName::Head) {
      if (!spec.isPosition) {
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      } else {
        // dst.lerp(localVector.fromArray(v2), f);
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }
    } else {
      if (!spec.isTop) {
        f *= (1 - avatar->idleWalkFactor);
      }

      interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
    }

    _clearXZ(spec.dst, spec.isPosition);
  }
  void _blendDance(AnimationMapping &spec, Avatar *avatar)
  {
    if (spec.isPosition) avatar->testString += "_blendDance, "; // test
    Animation *danceAnimation = avatar->danceMotiono[avatar->danceAnimationName == "" ? avatar->defaultDanceAnimationName : avatar->danceAnimationName]->animation;
    // const src2 = danceAnimation.interpolants[k];
    float t2 = fmod(AnimationMixer::nowS, danceAnimation->duration);
    // const v2 = src2.evaluate(t2);
    float *v2 = evaluateInterpolant(danceAnimation, spec.index, t2);

    float danceFactorS = avatar->danceFactor / avatar->crouchMaxTime;
    float f = min(max(danceFactorS, 0), 1);
    
    interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }
  void _blendNarutoRun(AnimationMapping &spec, Avatar *avatar)
  {
    if (spec.isPosition) avatar->testString += "_blendNarutoRun, "; // test
    // const narutoRunAnimation = narutoRunAnimations[defaultNarutoRunAnimation];
    Animation *narutoRunAnimation = avatar->motiono[avatar->defaultNarutoRunAnimation]->animation; // todo: use animationo directly. change animation.nam and add animation.fileName.
    // const src2 = narutoRunAnimation.interpolants[k];
    float t2 = fmod((avatar->narutoRunTime / 1000 * avatar->narutoRunTimeFactor), narutoRunAnimation->duration);
    // std::cout << " narutoRunTime: " << avatar->narutoRunTime  << " narutoRunTimeFactor: " << avatar->narutoRunTimeFactor << " duration: " << narutoRunAnimation->duration  << " t2: " << t2 << std::endl;
    // const v2 = src2.evaluate(t2);
    float *v2 = evaluateInterpolant(narutoRunAnimation, spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }
  void _blendSit(AnimationMapping &spec, Avatar *avatar)
  {
    if (spec.isPosition) avatar->testString += "_blendSit, "; // test
    // const sitAnimation = sitAnimations[avatar.sitAnimation || defaultSitAnimation];
    Animation *sitAnimation = avatar->motiono[avatar->sitAnimation == "" ? avatar->defaultSitAnimation : avatar->sitAnimation]->animation; // todo: use animationo directly. change animation.nam and add animation.fileName.
    // const src2 = sitAnimation.interpolants[k];
    // const v2 = src2.evaluate(1);
    float *v2 = evaluateInterpolant(sitAnimation, spec.index, 1);

    copyValue(spec.dst, v2, spec.isPosition);
  }
  void _blendJump(AnimationMapping &spec, Avatar *avatar)
  {
    if (spec.isPosition) avatar->testString += "_blendJump, "; // test
    float t2 = avatar->jumpTime / 1000;
    // const src2 = jumpAnimation.interpolants[k];
    // const v2 = src2.evaluate(t2);
    float *v2 = evaluateInterpolant(jumpAnimation, spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);

    if (avatar->holdState && spec.isArm) {
      Animation *holdAnimation = avatar->motiono["pick_up_idle"]->animation;
      float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
      float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);
      copyValue(spec.dst, v2, spec.isPosition);
    }
  }
  void _blendDoubleJump(AnimationMapping &spec, Avatar *avatar)
  {
    if (spec.isPosition) avatar->testString += "_blendDoubleJump, "; // test
    float t2 = avatar->doubleJumpTime / 1000;
    // const src2 = doubleJumpAnimation.interpolants[k];
    // const v2 = src2.evaluate(t2);
    float *v2 = evaluateInterpolant(doubleJumpAnimation, spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }
  void _blendFly(AnimationMapping &spec, Avatar *avatar)
  {
    if (avatar->flyState || (avatar->flyTime >= 0 && avatar->flyTime < 1000)) {
      if (spec.isPosition) avatar->testString += "_blendFly, "; // test
      float t2 = avatar->flyTime / 1000;
      // const f = avatar->flyState ? min(cubicBezier(t2), 1) : (1 - min(cubicBezier(t2), 1)); // todo: cubicBezier.
      float f = 1;
      // const src2 = floatAnimation.interpolants[k];
      // const v2 = src2.evaluate(t2 % floatAnimation.duration);
      float *v2 = evaluateInterpolant(floatAnimation, spec.index, fmod(t2, floatAnimation->duration));

      // lerpFn
      //   .call(
      //     dst,
      //     localQuaternion.fromArray(v2),
      //     f,
      //   );
      interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);

      if (avatar->holdState && spec.isArm) {
        Animation *holdAnimation = avatar->motiono["pick_up_idle"]->animation;
        float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
        float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);
        copyValue(spec.dst, v2, spec.isPosition);
      }
    }
  };
  void _blendFallLoop(AnimationMapping &spec, Avatar *avatar)
  {
    if (avatar->fallLoopFactor > 0) {
      if (spec.isPosition) avatar->testString += "_blendFallLoop, "; // test
      float t2 = (avatar->fallLoopTime / 1000);
      // const src2 = fallLoopAnimation.interpolants[k];
      // const v2 = src2.evaluate(t2);
      float *v2 = evaluateInterpolant(fallLoopAnimation, spec.index, t2);
      float f = clamp(t2 / 0.3, 0, 1);

      if (avatar->fallLoopFrom == "jump") {
        copyValue(spec.dst, v2, spec.isPosition);
      } else {
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }

      _clearXZ(spec.dst, spec.isPosition);
    }
  }
  void _blendLand(AnimationMapping &spec, Avatar *avatar)
  {
    if (!avatar->landWithMoving) {
      float animationSpeed = 0.75;
      float landTimeS = avatar->landTime / 1000;
      Animation *landingAnimation = animationo["landing.fbx"];
      float landingAnimationDuration = landingAnimation->duration / animationSpeed;
      float landFactor = landTimeS / landingAnimationDuration;

      if (landFactor > 0 && landFactor <= 1) {
        if (spec.isPosition) avatar->testString += "_blendLandMoveOff, "; // test
        float t2 = landTimeS * animationSpeed;
        // const src2 = landingAnimation->interpolants[k];
        // const v2 = src2.evaluate(t2);
        float *v2 = evaluateInterpolant(landingAnimation, spec.index, t2);

        float f = (landingAnimationDuration - landTimeS) / 0.05; // 0.05 = 3 frames
        f = clamp(f, 0, 1);

        // if (!spec.isPosition) {
        //   localQuaternion.fromArray(v2);
        //   dst.slerp(localQuaternion, f);
        // } else {
        //   localVector.fromArray(v2);
        //   _clearXZ(localVector, spec.isPosition);
        //   dst.lerp(localVector, f);
        // }
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }
    } else {
      if (spec.isPosition) avatar->testString += "_blendLandMoveOn, "; // test
      float animationSpeed = 0.95;
      float landTimeS = avatar->landTime / 1000;
      Animation *landingAnimation = animationo["landing 2.fbx"];
      float landingAnimationDuration = landingAnimation->duration / animationSpeed;
      float landFactor = landTimeS / landingAnimationDuration;

      if (landFactor > 0 && landFactor <= 1) {
        float t2 = landTimeS * animationSpeed;
        // const src2 = landingAnimation->interpolants[k];
        // const v2 = src2.evaluate(t2);
        float *v2 = evaluateInterpolant(landingAnimation, spec.index, t2);

        /* Calculating the time since the player landed on the ground. */
        float f3 = landTimeS / 0.1;
        f3 = clamp(f3, 0, 1);

        /* Calculating the time remaining until the landing animation is complete. */
        float f2 = (landingAnimationDuration - landTimeS) / 0.15;
        f2 = clamp(f2, 0, 1);

        float f = min(f3, f2);

        // if (!spec.isPosition) {
        //   localQuaternion2.fromArray(v2);
        //   dst.slerp(localQuaternion2, f);
        // } else {
        //   localVector2.fromArray(v2);
        //   dst.lerp(localVector2, f);
        //   _clearXZ(dst, spec.isPosition);
        // }
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
        _clearXZ(spec.dst, spec.isPosition);
      }
    }
  }
  void _blendActivate(AnimationMapping &spec, Avatar *avatar) // todo: full port
  {
    if (avatar->activateTime > 0)
    {
      if (spec.isPosition) avatar->testString += "_blendActivate, "; // test
      // if (spec.isPosition) std::cout << "activateAnimationName: " << avatar->activateAnimationName << std::endl;
      // Animation *activateAnimation = animationo2[avatar->activateAnimationName]; // todo: animationo
      Animation *activateAnimation = avatar->motiono[avatar->activateAnimationName]->animation;
      // Interpolant *src2 = activateAnimation->interpolants[spec.index];
      // const t2 = ((avatar.activateTime / 1000) * activateAnimations[defaultAnimation].speedFactor) % activateAnimation.duration; // todo: speedFactor
      float t2 = fmod((avatar->activateTime / 1000), activateAnimation->duration);
      // const v2 = src2.evaluate(t2);
      float *v2 = evaluateInterpolant(activateAnimation, spec.index, t2); // todo: src2->evaluate(t2);

      // const f = avatar.activateTime > 0 ? Math.min(cubicBezier(t2), 1) : (1 - Math.min(cubicBezier(t2), 1));
      float f = 1; // test: todo:

      // lerpFn
      //   .call(
      //     dst,
      //     localQuaternion.fromArray(v2),
      //     f,
      //   );

      interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
    }
  }
  float **AnimationMixer::update(float now, float nowS)
  {
    // AnimationMixer::now = now; // why can't set, cause idle and dance animatios play very fast ? use file variale instead ?
    AnimationMixer::nowS = nowS;

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
      animationValues[i] = rootNode->update(spec); // todo: set to spec.dst directly.
      spec.dst[0] = animationValues[i][0];
      spec.dst[1] = animationValues[i][1];
      spec.dst[2] = animationValues[i][2];
      if (!spec.isPosition) spec.dst[3] = animationValues[i][3];

      if (spec.isPosition) avatar->testString = ""; // test

      // note: Use exaclty same early return logic as js version, instead of all cascading, to prevent some bugs. But still want to use all cascading afterwards.
      if (avatar->doubleJumpState) {
        _blendDoubleJump(spec, this->avatar);
      } else if (avatar->jumpState) {
        _blendJump(spec, this->avatar);
      } else if (avatar->sitState) {
        _blendSit(spec, this->avatar);
      } else if (avatar->narutoRunState) {
        _blendNarutoRun(spec, this->avatar);
      } else if (avatar->danceFactor > 0) {
        _blendDance(spec, this->avatar);
      } else if (avatar->emoteFactor > 0) {
        _blendEmote(spec, this->avatar);
      } else if (
        avatar->useAnimationName != "" ||
        avatar->useAnimationComboName != "" || // todo: useAnimationComboNames, same logic as useAnimationEnvelopeNames.
        avatar->useAnimationEnvelopeNames.size() > 0
      ) {
        _blendUse(spec, this->avatar);
      } else if (avatar->hurtAnimationName != "") {
        _blendHurt(spec, this->avatar); // todo: move to highest priority.
      } else if (avatar->aimAnimationName != "") {
        _blendAim(spec, this->avatar);
      } else if (avatar->unuseAnimationName != "" && avatar->unuseTime >= 0) {
        _blendUnuse(spec, this->avatar);
      } else if (avatar->holdState) {
        _blendHold(spec, this->avatar);
      } else if (avatar->pickUpState) {
        _blendPickUp(spec, this->avatar);
      }/*  else {
        _handleDefault(spec, this->avatar); // todo:
      } */

      // cascading blending, in order to do transition between all kinds of aniamtions.
      _blendFly(spec, this->avatar);
      _blendFallLoop(spec, this->avatar);
      _blendLand(spec, this->avatar);
      // _blendActivate(spec, this->avatar);

      // if (spec.isPosition) std::cout << "testString: " << avatar->testString << std::endl; // test

      animationValues[i][0] = spec.dst[0];
      animationValues[i][1] = spec.dst[1];
      animationValues[i][2] = spec.dst[2];
      if (!spec.isPosition) animationValues[i][3] = spec.dst[3];
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
        evaluateTimeS = (AnimationMixer::nowS - this->startTime) * this->speed + this->timeBias;
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
        evaluateTimeS = fmod((AnimationMixer::nowS - this->startTime) * this->speed + this->timeBias, this->animation->duration);
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
          this->factor = (AnimationMixer::nowS - this->crossFadeStartTime) / this->crossFadeDuration;
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
          float factor = (AnimationMixer::nowS - this->crossFadeStartTime) / this->crossFadeDuration;
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
            this->factor = (AnimationMixer::nowS - this->crossFadeStartTime) / this->crossFadeDuration;
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
          this->factor = (AnimationMixer::nowS - this->crossFadeStartTime) / this->crossFadeDuration;
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
                localQuaternion[0] = value0[0];
                localQuaternion[1] = value0[1];
                localQuaternion[2] = value0[2];
                localQuaternion[3] = value0[3];
                if (spec.isArm)
                {
                  interpolateFlat(localQuaternion, 0, localQuaternion, 0, identityQuaternion, 0, this->arg, spec.isPosition);
                }

                Quat quat0(localQuaternion[0], localQuaternion[1], localQuaternion[2], localQuaternion[3]);
                Quat quat1(value1[0], value1[1], value1[2], value1[3]);
                quat0.premultiply(quat1);
                value1[0] = quat0.x;
                value1[1] = quat0.y;
                value1[2] = quat0.z;
                value1[3] = quat0.w;
              }
            }
            else
            {
              value1[0] = value0[0];
              value1[1] = value0[1];
              value1[2] = value0[2];
              value1[3] = value0[3];
            }
          }
          else if (this->index == 1) // emote animation
          {
            if (spec.index == BoneName::Spine || spec.index == BoneName::Chest || spec.index == BoneName::UpperChest || spec.index == BoneName::Neck || spec.index == BoneName::Head) {
              if (!spec.isPosition) {
                Quat quat0(value0[0], value0[1], value0[2], value0[3]);
                Quat quat1(value1[0], value1[1], value1[2], value1[3]);
                quat0.premultiply(quat1);
                value1[0] = quat0.x;
                value1[1] = quat0.y;
                value1[2] = quat0.z;
                value1[3] = quat0.w;
              } else {
                interpolateFlat(value1, 0, value0, 0, value1, 0, this->factor, spec.isPosition);
              }
            } else {
              float f = this->factor;
              if (!spec.isTop) {
                f *= (1 - this->arg); // arg: idleWalkFactor
              }

              interpolateFlat(value1, 0, value0, 0, value1, 0, f, spec.isPosition);
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
            interpolateFlat(result, 0, result, 0, value, 0, t, spec.isPosition);

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
    this->crossFadeStartTime = AnimationMixer::nowS;
    this->crossFadeDuration = duration;
    this->crossFadeTargetFactor = factor;
  }
  void AnimationNode::crossFadeSolitary(float duration, AnimationNode *targetNode)
  {
    this->isCrossFade = true;
    this->crossFadeStartTime = AnimationMixer::nowS;
    this->crossFadeDuration = duration;
    this->activeNode = targetNode;
  }
  void AnimationNode::play()
  {
    // todo: check if motion exists
    // std::cout << "play name: " << this->name << std::endl;
    this->weight = abs(this->weight);
    this->startTime = AnimationMixer::nowS;
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