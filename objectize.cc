#include <emscripten.h>
#include "geometry.h"
#include "compose.h"
#include "noise.h"
#include "march.h"
#include "collide.h"
#include "convex.h"
#include "earcut.h"
// #include <iostream>

#include <deque>
#include <map>

extern "C" {

// memory

EMSCRIPTEN_KEEPALIVE void *doMalloc(size_t size) {
  return malloc(size);
}

EMSCRIPTEN_KEEPALIVE void doFree(void *ptr) {
  free(ptr);
}

EMSCRIPTEN_KEEPALIVE ArenaAllocator *makeArenaAllocator(unsigned int size) {
  return new ArenaAllocator(size);
}

/* EMSCRIPTEN_KEEPALIVE FreeEntry *arenaAlloc(ArenaAllocator *arenaAllocator, unsigned int size) {
  return arenaAllocator->alloc(size);
}

EMSCRIPTEN_KEEPALIVE void arenaFree(ArenaAllocator *arenaAllocator, FreeEntry *entry) {
  arenaAllocator->free(entry);
} */

// vegetation

EMSCRIPTEN_KEEPALIVE GeometrySet *makeGeometrySet() {
  return doMakeGeometrySet();
}

EMSCRIPTEN_KEEPALIVE void loadBake(GeometrySet *geometrySet, unsigned char *data, unsigned int size) {
  doLoadBake(geometrySet, data, size);
}

EMSCRIPTEN_KEEPALIVE void getGeometry(GeometrySet *geometrySet, char *nameData, unsigned int nameSize, float **positions, float **uvs, unsigned int **indices, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIndices) {
  doGetGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, *numPositions, *numUvs, *numIndices);
}

EMSCRIPTEN_KEEPALIVE void getAnimalGeometry(GeometrySet *geometrySet, unsigned int hash, float **positions, unsigned char **colors, unsigned int **indices, float **heads, float **legs, unsigned int *numPositions, unsigned int *numColors, unsigned int *numIndices, unsigned int *numHeads, unsigned int *numLegs, float *headPivot, float *aabb) {
  doGetAnimalGeometry(geometrySet, hash, positions, colors, indices, heads, legs, *numPositions, *numColors, *numIndices, *numHeads, *numLegs, headPivot, aabb);
}

/* EMSCRIPTEN_KEEPALIVE void getMarchObjectStats(GeometrySet *geometrySet, Subparcel *subparcel, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIds, unsigned int *numIndices, unsigned int *numSkyLights, unsigned int *numTorchLights) {
  doGetMarchObjectStats(geometrySet, subparcel, *numPositions, *numUvs, *numIds, *numIndices, *numSkyLights, *numTorchLights);
}

EMSCRIPTEN_KEEPALIVE void marchObjects(GeometrySet *geometrySet, int x, int y, int z, Subparcel *subparcel, Subparcel *subparcels, unsigned int numSubparcels, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int indexOffset) {
  doMarchObjects(geometrySet, x, y, z, subparcel, subparcels, numSubparcels, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);
} */

// land

EMSCRIPTEN_KEEPALIVE float doGetHeight(int seed, float ax, float ay, float az, float baseHeight) {
  return getHeight(seed, ax, ay, az, baseHeight);
}

/* EMSCRIPTEN_KEEPALIVE void doNoise3(int seed, int x, int y, int z, float baseHeight, float wormRate, float wormRadiusBase, float wormRadiusRate, float objectsRate, float potentialDefault, void *subparcelByteOffset) {
  noise3(seed, x, y, z, baseHeight, wormRate, wormRadiusBase, wormRadiusRate, objectsRate, potentialDefault, subparcelByteOffset);
}

EMSCRIPTEN_KEEPALIVE void doMarchingCubes2(float meshId, int dims[3], float *potential, unsigned char *biomes, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float *positions, float *normals, float *uvs, unsigned char *aos, float *ids, unsigned char *skyLights, unsigned char *torchLights, unsigned int *positionIndex, unsigned int *normalIndex, unsigned int *uvIndex, unsigned int *aoIndex, unsigned int *idIndex, unsigned int *skyLightsIndex, unsigned int *torchLightsIndex, unsigned int &numOpaquePositions, unsigned int &numTransparentPositions, unsigned char *peeks) {
  marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions, normals, uvs, aos, ids, skyLights, torchLights, *positionIndex, *normalIndex, *uvIndex, *aoIndex, *idIndex, *skyLightsIndex, *torchLightsIndex, numOpaquePositions, numTransparentPositions, peeks);
} */

// physics

/* EMSCRIPTEN_KEEPALIVE void initPhysx() {
  doInitPhysx();
}
EMSCRIPTEN_KEEPALIVE void registerBakedGeometry(unsigned int meshId, PxDefaultMemoryOutputStream *writeStream, float *meshPosition, float *meshQuaternion, uintptr_t *result) {
  *result = (uintptr_t)doRegisterBakedGeometry(meshId, writeStream, meshPosition, meshQuaternion);
}
EMSCRIPTEN_KEEPALIVE void registerBoxGeometry(unsigned int meshId, float *position, float *quaternion, float w, float h, float d, uintptr_t *result) {
  *result = (uintptr_t)doRegisterBoxGeometry(meshId, position, quaternion, w, h, d);
}
EMSCRIPTEN_KEEPALIVE void registerCapsuleGeometry(unsigned int meshId, float *position, float *quaternion, float radius, float halfHeight, uintptr_t *result) {
  *result = (uintptr_t)doRegisterCapsuleGeometry(meshId, position, quaternion, radius, halfHeight);
} */
EMSCRIPTEN_KEEPALIVE PxDefaultMemoryOutputStream *bakeGeometry(Physicer *physicer, float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices) {
  return doBakeGeometry(physicer, positions, indices, numPositions, numIndices);
}
EMSCRIPTEN_KEEPALIVE PxDefaultMemoryOutputStream *bakeConvexGeometry(Physicer *physicer, float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices) {
  return doBakeConvexGeometry(physicer, positions, indices, numPositions, numIndices);
}
/* EMSCRIPTEN_KEEPALIVE void releaseBakedGeometry(PxDefaultMemoryOutputStream *writeStream) {
  delete writeStream;
}
EMSCRIPTEN_KEEPALIVE void unregisterGeometry(GeometrySpec *geometrySpec) {
  doUnregisterGeometry(geometrySpec);
} */
EMSCRIPTEN_KEEPALIVE void raycast(Tracker *tracker, float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int *hit, float *position, float *normal, float *distance, unsigned int *objectId, unsigned int *faceIndex, Vec *outPosition, Quat *outQuaternion) {
  doRaycast(tracker, origin, direction, meshPosition, meshQuaternion, *hit, position, normal, *distance, *objectId, *faceIndex, *outPosition, *outQuaternion);
}
EMSCRIPTEN_KEEPALIVE void collide(Tracker *tracker, float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int *hit, float *direction, unsigned int *grounded) {
  doCollide(tracker, radius, halfHeight, position, quaternion, meshPosition, meshQuaternion, maxIter, *hit, direction, *grounded);
}

// culling

/* EMSCRIPTEN_KEEPALIVE void *makeCuller() {
  return doMakeCuller();
}
EMSCRIPTEN_KEEPALIVE GroupSet *registerGroupSet(Culler *culler, int x, int y, int z, float r, unsigned char *peeks, Group *groups, unsigned int numGroups) {
  return doRegisterGroupSet(culler, x, y, z, r, peeks, groups, numGroups);
}
EMSCRIPTEN_KEEPALIVE void unregisterGroupSet(Culler *culler, GroupSet *groupSet) {
  doUnregisterGroupSet(culler, groupSet);
}
EMSCRIPTEN_KEEPALIVE void cull(Culler *culler, float *positionData, float *matrixData, float slabRadius, CullResult *cullResults, unsigned int *numCullResults) {
  doCull(culler, positionData, matrixData, slabRadius, cullResults, *numCullResults);
} */
EMSCRIPTEN_KEEPALIVE void tickCull(Tracker *tracker, float *positionData, float *matrixData, CullResult *landCullResults, unsigned int &numLandCullResults, CullResult *vegetationCullResults, unsigned int &numVegetationCullResults, CullResult *thingCullResults, unsigned int &numThingCullResults) {
  doTickCull(tracker, positionData, matrixData, landCullResults, numLandCullResults, vegetationCullResults, numVegetationCullResults, thingCullResults, numThingCullResults);
}

// tracking

EMSCRIPTEN_KEEPALIVE Tracker *makeTracker(
  int seed,
  unsigned int meshId,
  int chunkDistance,

  ArenaAllocator *landPositionsAllocator,
  ArenaAllocator *landNormalsAllocator,
  ArenaAllocator *landUvsAllocator,
  // ArenaAllocator *landBarycentricsAllocator,
  ArenaAllocator *landAosAllocator,
  ArenaAllocator *landIdsAllocator,
  ArenaAllocator *landSkyLightsAllocator,
  ArenaAllocator *landTorchLightsAllocator,

  ArenaAllocator *vegetationPositionsAllocator,
  ArenaAllocator *vegetationUvsAllocator,
  ArenaAllocator *vegetationIdsAllocator,
  ArenaAllocator *vegetationIndicesAllocator,
  ArenaAllocator *vegetationSkyLightsAllocator,
  ArenaAllocator *vegetationTorchLightsAllocator,

  ArenaAllocator *thingPositionsAllocator,
  ArenaAllocator *thingUvsAllocator,
  ArenaAllocator *thingAtlasUvsAllocator,
  ArenaAllocator *thingIdsAllocator,
  ArenaAllocator *thingIndicesAllocator,
  ArenaAllocator *thingSkyLightsAllocator,
  ArenaAllocator *thingTorchLightsAllocator
) {
  return new Tracker(
    seed,
    meshId,
    chunkDistance,

    landPositionsAllocator,
    landNormalsAllocator,
    landUvsAllocator,
    // landBarycentricsAllocator,
    landAosAllocator,
    landIdsAllocator,
    landSkyLightsAllocator,
    landTorchLightsAllocator,

    vegetationPositionsAllocator,
    vegetationUvsAllocator,
    vegetationIdsAllocator,
    vegetationIndicesAllocator,
    vegetationSkyLightsAllocator,
    vegetationTorchLightsAllocator,

    thingPositionsAllocator,
    thingUvsAllocator,
    thingAtlasUvsAllocator,
    thingIdsAllocator,
    thingIndicesAllocator,
    thingSkyLightsAllocator,
    thingTorchLightsAllocator
  );
}

EMSCRIPTEN_KEEPALIVE void tickTracker(Tracker *tracker, ThreadPool *threadPool, GeometrySet *geometrySet, float x, float y, float z) {
  tracker->updateNeededCoords(threadPool, geometrySet, x, y, z);
}

EMSCRIPTEN_KEEPALIVE void doChunk(float meshId, int dims[3], float *potential, unsigned char *biomes, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float *positions, float *normals, float *uvs, /*float *barycentrics,*/ unsigned char *aos, float *ids, unsigned char *skyLights, unsigned char *torchLights, unsigned int *positionIndex, unsigned int *normalIndex, unsigned int *uvIndex, /*unsigned int *barycentricIndex,*/ unsigned int *aoIndex, unsigned int *idIndex, unsigned int *skyLightsIndex, unsigned int *torchLightsIndex, unsigned int &numOpaquePositions, unsigned int &numTransparentPositions, unsigned char *peeks) {
  marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions, normals, uvs, /*barycentrics,*/ aos, ids, skyLights, torchLights, *positionIndex, *normalIndex, *uvIndex, /**barycentricIndex,*/ *aoIndex, *idIndex, *skyLightsIndex, *torchLightsIndex, numOpaquePositions, numTransparentPositions, peeks);
}

// convex hull

EMSCRIPTEN_KEEPALIVE ConvexHullResult *convexHull(float *positions, unsigned int numPositions, float *cameraPosition) {
  return doConvexHull(positions, numPositions, cameraPosition);
}

// earcut

EMSCRIPTEN_KEEPALIVE EarcutResult *earcut(Tracker *tracker, float *positions, unsigned int numPositions, float *holes, unsigned int *holeCounts, unsigned int numHoleCounts, float *points, unsigned int numPoints, float z, float *zs) {
  return doEarcut(tracker, positions, numPositions, holes, holeCounts, numHoleCounts, points, numPoints, z, zs);
}

// requests

EMSCRIPTEN_KEEPALIVE ThreadPool *makeThreadPool(unsigned int numThreads) {
  return new ThreadPool(numThreads);
}

/* unsigned int nextMessageId = 0;
EMSCRIPTEN_KEEPALIVE unsigned int pushRequest(ThreadPool *threadPool, Message *message) {
  unsigned int messageId = ++nextMessageId;
  message->id = messageId;
  threadPool->inbox.queue(message);
  return messageId;
}

EMSCRIPTEN_KEEPALIVE unsigned int popResponse(ThreadPool *threadPool) {
  ResponseMessage *responseMessage = threadPool->outbox.pop();
  if (responseMessage) {
    unsigned int messageId = responseMessage->id;
    delete responseMessage;
    return messageId;
  } else {
    return 0;
  }
} */

EMSCRIPTEN_KEEPALIVE void tick(ThreadPool *threadPool, unsigned char *ptr, unsigned int numEntries, unsigned char *outPtr, unsigned int *outNumEntries) {
  {
    std::vector<Message *> outMessages;
    threadPool->outbox.popAll(outMessages);
    // std::cout << "out messages a " << outMessages.size() << std::endl;
    unsigned int index = 0;
    int i = 0;
    for (Message *outMessage : outMessages) {
      Message *clientMessage = (Message *)(outPtr + index);

      unsigned int offsetCount = outMessage->count;
      unsigned int offsetSize = sizeof(Message) - 4 + offsetCount*sizeof(unsigned int);
      memcpy(clientMessage, outMessage, offsetSize);

      // std::cout << "out id " << i << " " << outMessages.size() << " " << clientMessage->id << " " << index << " " << (unsigned int)clientMessage << std::endl;
      /* if (!clientMessage->id || clientMessage->id > 100) {
        std::cout << "no id a " << clientMessage->id << " " << index << std::endl;
        abort();
      } */

      index += offsetSize;

      free(outMessage);
      i++;
    }
    // std::cout << "out messages b " << outMessages.size() << std::endl;
    *outNumEntries = outMessages.size();
  }
  {
    Message *message;
    while ((message = threadPool->dependencyInbox.pop())) {
      message->priority = 1;
      threadPool->inbox.queue(message);
    }
  }
  {
    std::vector<Message *> inMessages(numEntries);
    unsigned int index = 0;
    for (unsigned int i = 0; i < numEntries; i++) {
      Message *clientMessage = (Message *)(ptr + index);

      unsigned int offsetCount = clientMessage->count;
      unsigned int offsetSize = sizeof(Message) - 4 + offsetCount*sizeof(unsigned int);

      Message *newMessage = (Message *)malloc(offsetSize);
      memcpy(newMessage, clientMessage, offsetSize);
      // std::cout << "in id " << newMessage->id << std::endl;
      /* if (!newMessage->id || newMessage->id > 100) {
        std::cout << "no id b " << newMessage->id << " " << index << std::endl;
        abort();
      } */

      inMessages[i] = newMessage;
      index += offsetSize;
    }
    threadPool->inbox.queueAll(std::move(inMessages));
  }
}

bool checkSubparcelIndicesLive(Tracker *tracker, const std::vector<int> indices) {
  std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
  for (int index : indices) {
    auto iter = tracker->subparcels.find(index);
    if (iter != tracker->subparcels.end()) {
      std::shared_ptr<Subparcel> &subparcel = iter->second;
      if (!subparcel->live) {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

std::function<void(ThreadPool *, Message *)> METHOD_FNS[] = {
  [](ThreadPool *threadPool, Message *Message) -> void { // makeArenaAllocator
    abort();
    /* unsigned int index = 0;
    unsigned int size = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    ArenaAllocator **arenaAllocator = (ArenaAllocator **)(Message->args + index);
    index += sizeof(ArenaAllocator *);

    *arenaAllocator = makeArenaAllocator(size); v*/
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // arenaAlloc
    abort();
    /* unsigned int index = 0;
    ArenaAllocator *arenaAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    unsigned int size = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    FreeEntry **entry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry **);

    *entry = arenaAlloc(arenaAllocator, size); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // arenaFree
    abort();
    /* unsigned int index = 0;
    ArenaAllocator *arenaAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    FreeEntry *entry = *((FreeEntry **)(Message->args + index));
    index += sizeof(FreeEntry *);

    arenaFree(arenaAllocator, entry); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // makeGeometrySet
    abort();
    /* GeometrySet **geometrySet = (GeometrySet **)Message->args;
    *geometrySet = new GeometrySet(); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // loadBake
    GeometrySet *geometrySet = *((GeometrySet **)Message->args);
    unsigned char *data = *((unsigned char **)(Message->args + sizeof(GeometrySet *)));
    unsigned int dataSize = *((unsigned int *)(Message->args + sizeof(GeometrySet *) + sizeof(unsigned char *)));
    // std::cout << "load bake 1 " << (unsigned int)(void *)geometrySet << " " << (unsigned int)data << " " << dataSize << std::endl;
    loadBake(geometrySet, data, dataSize);
    // std::cout << "load bake 2" << std::endl;

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // getGeometry
    unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *nameData = *((char **)(Message->args + index));
    index += sizeof(char *);
    unsigned int nameSize = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(Message->args + index);
    index += sizeof(float **);
    float **uvs = (float **)(Message->args + index);
    index += sizeof(float **);
    unsigned int **indices = (unsigned int **)(Message->args + index);
    index += sizeof(unsigned int **);
    unsigned int *numPositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numUvs = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);

    getGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, numPositions, numUvs, numIndices);

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // getAnimalGeometry
    unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    unsigned int hash = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(Message->args + index);
    index += sizeof(float **);
    unsigned char **colors = (unsigned char **)(Message->args + index);
    index += sizeof(unsigned char **);
    unsigned int **indices = (unsigned int **)(Message->args + index);
    index += sizeof(unsigned int **);
    float **heads = (float **)(Message->args + index);
    index += sizeof(float **);
    float **legs = (float **)(Message->args + index);
    index += sizeof(float **);
    unsigned int *numPositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numColors = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numHeads = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numLegs = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    float *headPivot = (float *)(Message->args + index);
    index += 3*sizeof(float *);
    float *aabb = (float *)(Message->args + index);
    index += 6*sizeof(float *);

    getAnimalGeometry(geometrySet, hash, positions, colors, indices, heads, legs, numPositions, numColors, numIndices, numHeads, numLegs, headPivot, aabb);

    threadPool->outbox.push(Message);

    /* unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *nameData = *((char **)(Message->args + index));
    index += sizeof(char *);
    unsigned int nameSize = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(Message->args + index);
    index += sizeof(float **);
    float **uvs = (float **)(Message->args + index);
    index += sizeof(float **);
    unsigned int **indices = (unsigned int **)(Message->args + index);
    index += sizeof(unsigned int **);
    unsigned int *numPositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numUvs = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);

    getGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, numPositions, numUvs, numIndices); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // marchObjects
    abort();
    /* unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    int x = *((int *)(Message->args + index));
    index += sizeof(int);
    int y = *((int *)(Message->args + index));
    index += sizeof(int);
    int z = *((int *)(Message->args + index));
    index += sizeof(int);

    // std::cout << "march objects a " << x << " " << y << " " << z << std::endl;

    Subparcel *subparcel = *((Subparcel **)(Message->args + index));
    index += sizeof(Subparcel *);
    Subparcel *subparcels = *((Subparcel **)(Message->args + index));
    index += sizeof(Subparcel *);
    unsigned int numSubparcels = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    ArenaAllocator *positionsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *uvsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *idsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *indicesAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *skyLightsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *torchLightsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    FreeEntry **positionsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **uvsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **idsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **indicesEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **skyLightsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **torchLightsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);

    // std::cout << "march objects ax " << (unsigned int)(void *)geometrySet << " " << (unsigned int)(void *)marchObjects << " " << numMarchObjects << std::endl;

    unsigned int numPositions;
    unsigned int numUvs;
    unsigned int numIds;
    unsigned int numIndices;
    unsigned int numSkyLights;
    unsigned int numTorchLights;
    doGetMarchObjectStats(geometrySet, subparcel, numPositions, numUvs, numIds, numIndices, numSkyLights, numTorchLights);

    // std::cout << "march objects b " << numPositions << " " << numUvs << " " << numIds << " " << numIndices << " " << numSkyLights << " " << numTorchLights << " " << (unsigned int)(void *)positionsAllocator << std::endl;

    *positionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
    if (!*positionsEntry) {
      std::cout << "could not allocate marchObjects positions" << std::endl;
      abort();
    }
    *uvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
    if (!*uvsEntry) {
      std::cout << "could not allocate marchObjects uvs" << std::endl;
      abort();
    }
    *idsEntry = idsAllocator->alloc(numIds*sizeof(float));
    if (!*idsEntry) {
      std::cout << "could not allocate marchObjects ids" << std::endl;
      abort();
    }
    *indicesEntry = indicesAllocator->alloc(numIndices*sizeof(unsigned int));
    if (!*indicesEntry) {
      std::cout << "could not allocate marchObjects indices" << std::endl;
      abort();
    }
    *skyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
    if (!*skyLightsEntry) {
      std::cout << "could not allocate marchObjects skyLights" << std::endl;
      abort();
    }
    *torchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
    if (!*torchLightsEntry) {
      std::cout << "could not allocate marchObjects torchLights" << std::endl;
      abort();
    }

    // std::cout << "march objects c " << numPositions << " " << numUvs << " " << numIds << " " << numIndices << " " << numSkyLights << " " << numTorchLights << std::endl;

    float *positions = (float *)(positionsAllocator->data + (*positionsEntry)->start);
    float *uvs = (float *)(uvsAllocator->data + (*uvsEntry)->start);
    float *ids = (float *)(idsAllocator->data + (*idsEntry)->start);
    unsigned int *indices = (unsigned int *)(indicesAllocator->data + (*indicesEntry)->start);
    unsigned char *skyLights = (unsigned char *)(skyLightsAllocator->data + (*skyLightsEntry)->start);
    unsigned char *torchLights = (unsigned char *)(torchLightsAllocator->data + (*torchLightsEntry)->start);

    unsigned int indexOffset = (*positionsEntry)->start/sizeof(float)/3;

    doMarchObjects(geometrySet, x, y, z, subparcel, subparcels, numSubparcels, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);

    // std::cout << "march objects d " << numIndices << " " << indices[0] << " " << indices[1] << " " << indices[2] << " " << (unsigned int)(void *)positionsAllocator << " " << (unsigned int)(void *)indicesAllocator << " " << (unsigned int)(void *)indicesAllocator->data << " " << (*indicesEntry)->start << std::endl; */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // getHeight
    unsigned int index = 0;
    int seed = *((int *)(Message->args + index));
    index += sizeof(int);
    float x = *((float *)(Message->args + index));
    index += sizeof(float);
    float y = *((float *)(Message->args + index));
    index += sizeof(float);
    float z = *((float *)(Message->args + index));
    index += sizeof(float);
    float baseHeight = *((float *)(Message->args + index));
    index += sizeof(float);
    float *height = (float *)(Message->args + index);
    index += sizeof(float *);
    *height = getHeight(seed, x, y, z, baseHeight);

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // noise
    abort();
    /* unsigned int index = 0;

    int seed = *((int *)(Message->args + index));
    index += sizeof(int);

    float x = *((float *)(Message->args + index));
    index += sizeof(float);
    float y = *((float *)(Message->args + index));
    index += sizeof(float);
    float z = *((float *)(Message->args + index));
    index += sizeof(float);

    float baseHeight = *((float *)(Message->args + index));
    index += sizeof(float);
    float wormRate = *((float *)(Message->args + index));
    index += sizeof(float);
    float wormRadiusBase = *((float *)(Message->args + index));
    index += sizeof(float);
    float wormRadiusRate = *((float *)(Message->args + index));
    index += sizeof(float);
    float objectsRate = *((float *)(Message->args + index));
    index += sizeof(float);
    float potentialDefault = *((float *)(Message->args + index));
    index += sizeof(float);

    void *subparcelByteOffset = *((void **)(Message->args + index));
    index += sizeof(void *);

    noise3(seed, x, y, z, baseHeight, wormRate, wormRadiusBase, wormRadiusRate, objectsRate, potentialDefault, subparcelByteOffset); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // marchingCubes
    abort();
    /* unsigned int index = 0;

    float meshId = *(float *)(Message->args + index);
    index += sizeof(float);

    int *dims = (int *)(Message->args + index);
    index += 3*sizeof(int);

    float *potential = *((float **)(Message->args + index));
    index += sizeof(float *);
    unsigned char *biomes = *((unsigned char **)(Message->args + index));
    index += sizeof(unsigned char *);
    char *heightfield = *((char **)(Message->args + index));
    index += sizeof(char *);
    unsigned char *lightfield = *((unsigned char **)(Message->args + index));
    index += sizeof(unsigned char *);

    float *shift = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float *scale = (float *)(Message->args + index);
    index += 3*sizeof(float);

    ArenaAllocator *positionsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *normalsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *uvsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    // ArenaAllocator *barycentricsAllocator = *((ArenaAllocator **)(Message->args + index));
    // index += sizeof(ArenaAllocator *);
    ArenaAllocator *aosAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *idsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *skyLightsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *torchLightsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *peeksAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);

    FreeEntry **positionsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **normalsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **uvsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    // FreeEntry **barycentricsEntry = (FreeEntry **)(Message->args + index);
    // index += sizeof(FreeEntry *);
    FreeEntry **aosEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **idsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **skyLightsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **torchLightsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **peeksEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);

    unsigned int *numOpaquePositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numTransparentPositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);

    constexpr int bufferSize = 1024*1024;
    std::vector<float> positions(bufferSize);
    std::vector<float> normals(bufferSize);
    std::vector<float> uvs(bufferSize);
    // std::vector<float> barycentrics(bufferSize);
    std::vector<unsigned char> aos(bufferSize);
    std::vector<float> ids(bufferSize);
    std::vector<unsigned char> skyLights(bufferSize);
    std::vector<unsigned char> torchLights(bufferSize);
    constexpr unsigned int numPeeks = 15;
    std::vector<unsigned char> peeks(numPeeks);
    unsigned int numPositions;
    unsigned int numNormals;
    unsigned int numUvs;
    // unsigned int numBarycentrics;
    unsigned int numAos;
    unsigned int numIds;
    unsigned int numSkyLights;
    unsigned int numTorchLights;
    marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions.data(), normals.data(), uvs.data(), aos.data(), ids.data(), skyLights.data(), torchLights.data(), numPositions, numNormals, numUvs, numAos, numIds, numSkyLights, numTorchLights, *numOpaquePositions, *numTransparentPositions, peeks.data());

    *positionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
    if (!*positionsEntry) {
      std::cout << "could not allocate marchingCubes positions" << std::endl;
      abort();
    }
    *normalsEntry = normalsAllocator->alloc(numNormals*sizeof(float));
    if (!*normalsEntry) {
      std::cout << "could not allocate marchingCubes normals" << std::endl;
      abort();
    }
    *uvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
    if (!*uvsEntry) {
      std::cout << "could not allocate marchingCubes uvs" << std::endl;
      abort();
    }
    // *barycentricsEntry = barycentricsAllocator->alloc(numBarycentrics*sizeof(float));
    // if (!*barycentricsEntry) {
      // std::cout << "could not allocate marchingCubes barycentrics" << std::endl;
      // abort();
    // }
    *aosEntry = aosAllocator->alloc(numAos*sizeof(float));
    if (!*aosEntry) {
      std::cout << "could not allocate marchingCubes aos" << std::endl;
      abort();
    }
    *idsEntry = idsAllocator->alloc(numIds*sizeof(float));
    if (!*idsEntry) {
      std::cout << "could not allocate marchingCubes ids" << std::endl;
      abort();
    }
    *skyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
    if (!*skyLightsEntry) {
      std::cout << "could not allocate marchingCubes skyLights" << std::endl;
      abort();
    }
    *torchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
    if (!*torchLightsEntry) {
      std::cout << "could not allocate marchingCubes torchLights" << std::endl;
      abort();
    }
    *peeksEntry = peeksAllocator->alloc(numPeeks*sizeof(unsigned char));
    if (!*peeksEntry) {
      std::cout << "could not allocate marchingCubes peeks" << std::endl;
      abort();
    }

    memcpy(positionsAllocator->data + (*positionsEntry)->start, positions.data(), numPositions*sizeof(float));
    memcpy(normalsAllocator->data + (*normalsEntry)->start, normals.data(), numNormals*sizeof(float));
    memcpy(uvsAllocator->data + (*uvsEntry)->start, uvs.data(), numUvs*sizeof(float));
    // memcpy(barycentricsAllocator->data + (*barycentricsEntry)->start, barycentrics.data(), numBarycentrics*sizeof(float));
    memcpy(aosAllocator->data + (*aosEntry)->start, aos.data(), numAos*sizeof(unsigned char));
    memcpy(idsAllocator->data + (*idsEntry)->start, ids.data(), numIds*sizeof(float));
    memcpy(skyLightsAllocator->data + (*skyLightsEntry)->start, skyLights.data(), numSkyLights*sizeof(unsigned char));
    memcpy(torchLightsAllocator->data + (*torchLightsEntry)->start, torchLights.data(), numTorchLights*sizeof(unsigned char));
    memcpy(peeksAllocator->data + (*peeksEntry)->start, peeks.data(), numPeeks*sizeof(unsigned char)); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // bakeGeometry
    abort();
    /* unsigned int index = 0;

    float *positions = *((float **)(Message->args + index));
    index += sizeof(float *);

    unsigned int *indices = *((unsigned int **)(Message->args + index));
    index += sizeof(unsigned int *);

    unsigned int numPositions = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    unsigned int numIndices = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    PxDefaultMemoryOutputStream **writeStream = (PxDefaultMemoryOutputStream **)(Message->args + index);
    index += sizeof(PxDefaultMemoryOutputStream *);

    // std::cout << "bake 1" << std::endl;
    bakeGeometry(positions, indices, numPositions, numIndices, writeStream);
    // std::cout << "bake 2" << std::endl; */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // getSubparcel
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    int x = *((int *)(Message->args + index));
    index += sizeof(int);
    int y = *((int *)(Message->args + index));
    index += sizeof(void *);
    int z = *((int *)(Message->args + index));
    index += sizeof(void *);

    std::shared_ptr<Subparcel> subparcel;
    {
      int subparcelIndex = getSubparcelIndex(x, y, z);
      std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
      auto subparcelIter = tracker->subparcels.find(subparcelIndex);
      if (subparcelIter != tracker->subparcels.end()) {
        subparcel = subparcelIter->second;
      }
    }
    std::shared_ptr<Subparcel> *subparcelPtr;
    if (subparcel) {
      subparcelPtr = new std::shared_ptr<Subparcel>(subparcel);
    } else {
      subparcelPtr = nullptr;
    }

    index = 0;
    *((std::shared_ptr<Subparcel> **)(Message->args + index)) = subparcelPtr;
    index += sizeof(std::shared_ptr<Subparcel> *);
    *((Subparcel **)(Message->args + index)) = subparcel.get();
    index += sizeof(Subparcel *);

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // releaseSubparcel
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    std::shared_ptr<Subparcel> *subparcelSharedPtr = *((std::shared_ptr<Subparcel> **)(Message->args + index));
    index += sizeof(std::shared_ptr<Subparcel> *);

    {
      std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
      if ((*subparcelSharedPtr)->live) {
        tracker->subparcels[(*subparcelSharedPtr)->coord.index] = *subparcelSharedPtr;
        tracker->loadingSubparcels.erase((*subparcelSharedPtr)->coord.index);
      }
    }
    delete subparcelSharedPtr;

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // chunk
    unsigned int index = 0;
    int seed = *((int *)(Message->args + index));
    index += sizeof(int);
    void *trackerByteOffset = *((void **)(Message->args + index));
    index += sizeof(void *);
    void *geometrySetByteOffset = *((void **)(Message->args + index));
    index += sizeof(void *);
    void *subparcelSharedPtrByteOffset = *((void **)(Message->args + index));
    index += sizeof(void *);

    Tracker *tracker = (Tracker *)trackerByteOffset;
    GeometrySet *geometrySet = (GeometrySet *)geometrySetByteOffset;
    std::shared_ptr<Subparcel> *subparcelSharedPtr = (std::shared_ptr<Subparcel> *)subparcelSharedPtrByteOffset;

    if ((*subparcelSharedPtr)->live) { // check for abort
      std::shared_ptr<Subparcel> subparcel(*subparcelSharedPtr);

      {
        constexpr float baseHeight = (float)PARCEL_SIZE/2.0f-10.0f;
        constexpr float wormRate = 2;
        constexpr float wormRadiusBase = 2;
        constexpr float wormRadiusRate = 2;
        constexpr float objectsRate = 3;
        constexpr float potentialDefault = -0.5;

        // std::cout << "execute chunk " << subparcel->coord.x << " " << subparcel->coord.y << " " << subparcel->coord.z << std::endl;
        noise3(seed, subparcel->coord.x, subparcel->coord.y, subparcel->coord.z, baseHeight, wormRate, wormRadiusBase, wormRadiusRate, objectsRate, potentialDefault, subparcel.get());
      }

      float *landPositions;
      unsigned int numLandPositions;
      bool textureUpdated;
      {
        float meshId = tracker->meshId;
        int dims[3] = {
          SUBPARCEL_SIZE,
          SUBPARCEL_SIZE,
          SUBPARCEL_SIZE,
        };
        float *potential = subparcel->potentials;
        unsigned char *biomes = subparcel->biomes;
        char *heightfield = subparcel->heightfield;
        unsigned char *lightfield = subparcel->lightfield;
        float shift[3] = {
          (float)subparcel->coord.x*(float)SUBPARCEL_SIZE,
          (float)subparcel->coord.y*(float)SUBPARCEL_SIZE,
          (float)subparcel->coord.z*(float)SUBPARCEL_SIZE,
        };
        float scale[3] = {1, 1, 1};

        ArenaAllocator *positionsAllocator = tracker->landPositionsAllocator;
        ArenaAllocator *normalsAllocator = tracker->landNormalsAllocator;
        ArenaAllocator *uvsAllocator = tracker->landUvsAllocator;
        // ArenaAllocator *barycentricsAllocator = tracker->landBarycentricsAllocator;
        ArenaAllocator *aosAllocator = tracker->landAosAllocator;
        ArenaAllocator *idsAllocator = tracker->landIdsAllocator;
        ArenaAllocator *skyLightsAllocator = tracker->landSkyLightsAllocator;
        ArenaAllocator *torchLightsAllocator = tracker->landTorchLightsAllocator;

        unsigned int numOpaquePositions;
        unsigned int numTransparentPositions;

        constexpr int bufferSize = 1024*1024;
        std::vector<float> positions(bufferSize);
        std::vector<float> normals(bufferSize);
        std::vector<float> uvs(bufferSize);
        // std::vector<float> barycentrics(bufferSize);
        std::vector<unsigned char> aos(bufferSize);
        std::vector<float> ids(bufferSize);
        std::vector<unsigned char> skyLights(bufferSize);
        std::vector<unsigned char> torchLights(bufferSize);
        constexpr unsigned int numPeeks = 15;
        std::vector<unsigned char> peeks(numPeeks);
        unsigned int numPositions;
        unsigned int numNormals;
        unsigned int numUvs;
        // unsigned int numBarycentrics;
        unsigned int numAos;
        unsigned int numIds;
        unsigned int numSkyLights;
        unsigned int numTorchLights;
        marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions.data(), normals.data(), uvs.data(), /*barycentrics.data(),*/ aos.data(), ids.data(), skyLights.data(), torchLights.data(), numPositions, numNormals, numUvs, /*numBarycentrics,*/ numAos, numIds, numSkyLights, numTorchLights, numOpaquePositions, numTransparentPositions, peeks.data());

        subparcel->landPositionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
        if (!subparcel->landPositionsEntry) {
          std::cout << "could not allocate chunk marchingCubes positions" << std::endl;
          abort();
        }
        subparcel->landNormalsEntry = normalsAllocator->alloc(numNormals*sizeof(float));
        if (!subparcel->landNormalsEntry) {
          std::cout << "could not allocate chunk marchingCubes normals" << std::endl;
          abort();
        }
        subparcel->landUvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
        if (!subparcel->landUvsEntry) {
          std::cout << "could not allocate chunk marchingCubes uvs" << std::endl;
          abort();
        }
        /* subparcel->landBarycentricsEntry = barycentricsAllocator->alloc(numBarycentrics*sizeof(float)); // XXX maybe not needed
        if (!subparcel->landBarycentricsEntry) {
          std::cout << "could not allocate chunk marchingCubes barycentrics" << std::endl;
          abort();
        } */
        subparcel->landAosEntry = aosAllocator->alloc(numAos*sizeof(unsigned char));
        if (!subparcel->landAosEntry) {
          std::cout << "could not allocate chunk marchingCubes aos" << std::endl;
          abort();
        }
        subparcel->landIdsEntry = idsAllocator->alloc(numIds*sizeof(float));
        if (!subparcel->landIdsEntry) {
          std::cout << "could not allocate chunk marchingCubes ids" << std::endl;
          abort();
        }
        subparcel->landSkyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
        if (!subparcel->landSkyLightsEntry) {
          std::cout << "could not allocate chunk marchingCubes skyLights" << std::endl;
          abort();
        }
        subparcel->landTorchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
        if (!subparcel->landTorchLightsEntry) {
          std::cout << "could not allocate chunk marchingCubes torchLights" << std::endl;
          abort();
        }

        memcpy(positionsAllocator->data + subparcel->landPositionsEntry->spec.start, positions.data(), numPositions*sizeof(float));
        memcpy(normalsAllocator->data + subparcel->landNormalsEntry->spec.start, normals.data(), numNormals*sizeof(float));
        memcpy(uvsAllocator->data + subparcel->landUvsEntry->spec.start, uvs.data(), numUvs*sizeof(float));
        // memcpy(barycentricsAllocator->data + subparcel->landBarycentricsEntry->start, barycentrics.data(), numBarycentrics*sizeof(float));
        memcpy(aosAllocator->data + subparcel->landAosEntry->spec.start, aos.data(), numAos*sizeof(unsigned char));
        memcpy(idsAllocator->data + subparcel->landIdsEntry->spec.start, ids.data(), numIds*sizeof(float));
        memcpy(skyLightsAllocator->data + subparcel->landSkyLightsEntry->spec.start, skyLights.data(), numSkyLights*sizeof(unsigned char));
        memcpy(torchLightsAllocator->data + subparcel->landTorchLightsEntry->spec.start, torchLights.data(), numTorchLights*sizeof(unsigned char));
        memcpy(subparcel->peeks, peeks.data(), numPeeks*sizeof(unsigned char));

        // groups
        subparcel->landGroups[0].start = subparcel->landPositionsEntry->spec.start/sizeof(float)/3;
        subparcel->landGroups[0].count = numOpaquePositions/3;
        subparcel->landGroups[0].materialIndex = 0;
        subparcel->landGroups[1].start = subparcel->landGroups[0].start + subparcel->landGroups[0].count;
        subparcel->landGroups[1].count = numTransparentPositions/3;
        subparcel->landGroups[1].materialIndex = 1;

        // latch
        landPositions = (float *)(positionsAllocator->data + subparcel->landPositionsEntry->spec.start);
        numLandPositions = numOpaquePositions;
      }
      {
        // std::cout << "march objects ax " << (unsigned int)(void *)geometrySet << " " << (unsigned int)(void *)marchObjects << " " << numMarchObjects << std::endl;

        unsigned int numPositions;
        unsigned int numUvs;
        unsigned int numIds;
        unsigned int numIndices;
        unsigned int numSkyLights;
        unsigned int numTorchLights;
        doGetMarchObjectStats(geometrySet, subparcel.get(), numPositions, numUvs, numIds, numIndices, numSkyLights, numTorchLights);

        ArenaAllocator *positionsAllocator = tracker->vegetationPositionsAllocator;
        ArenaAllocator *uvsAllocator = tracker->vegetationUvsAllocator;
        ArenaAllocator *idsAllocator = tracker->vegetationIdsAllocator;
        ArenaAllocator *indicesAllocator = tracker->vegetationIndicesAllocator;
        ArenaAllocator *skyLightsAllocator = tracker->vegetationSkyLightsAllocator;
        ArenaAllocator *torchLightsAllocator = tracker->vegetationTorchLightsAllocator;

        subparcel->vegetationPositionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
        if (!subparcel->vegetationPositionsEntry) {
          std::cout << "could not allocate chunk marchObjects positions" << std::endl;
          abort();
        }
        subparcel->vegetationUvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
        if (!subparcel->vegetationUvsEntry) {
          std::cout << "could not allocate chunk marchObjects uvs" << std::endl;
          abort();
        }
        subparcel->vegetationIdsEntry = idsAllocator->alloc(numIds*sizeof(float));
        if (!subparcel->vegetationIdsEntry) {
          std::cout << "could not allocate chunk marchObjects ids" << std::endl;
          abort();
        }
        subparcel->vegetationIndicesEntry = indicesAllocator->alloc(numIndices*sizeof(unsigned int));
        if (!subparcel->vegetationIndicesEntry) {
          std::cout << "could not allocate chunk marchObjects indices" << std::endl;
          abort();
        }
        subparcel->vegetationSkyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
        if (!subparcel->vegetationSkyLightsEntry) {
          std::cout << "could not allocate chunk marchObjects skyLights" << std::endl;
          abort();
        }
        subparcel->vegetationTorchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
        if (!subparcel->vegetationTorchLightsEntry) {
          std::cout << "could not allocate chunk marchObjects torchLights" << std::endl;
          abort();
        }

        float *positions = (float *)(positionsAllocator->data + subparcel->vegetationPositionsEntry->spec.start);
        float *uvs = (float *)(uvsAllocator->data + subparcel->vegetationUvsEntry->spec.start);
        float *ids = (float *)(idsAllocator->data + subparcel->vegetationIdsEntry->spec.start);
        unsigned int *indices = (unsigned int *)(indicesAllocator->data + subparcel->vegetationIndicesEntry->spec.start);
        unsigned char *skyLights = (unsigned char *)(skyLightsAllocator->data + subparcel->vegetationSkyLightsEntry->spec.start);
        unsigned char *torchLights = (unsigned char *)(torchLightsAllocator->data + subparcel->vegetationTorchLightsEntry->spec.start);

        /* slab => (slab.position.byteOffset - geometry.attributes.position.array.byteOffset)/Float32Array.BYTES_PER_ELEMENT;
        const indexOffset = vegetationMesh.getSlabPositionOffset(slab)/3;
        for (let i = 0; i < spec.indices.length; i++) {
          spec.indices[i] += indexOffset;
        } */
        Subparcel *subparcels = nullptr;
        unsigned int numSubparcels = 0;
        unsigned int indexOffset = subparcel->vegetationPositionsEntry->spec.start/sizeof(float)/3;
        doMarchObjects(geometrySet, subparcel->coord.x, subparcel->coord.y, subparcel->coord.z, subparcel.get(), subparcels, numSubparcels, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);

        // groups
        subparcel->vegetationGroups[0].start = subparcel->vegetationIndicesEntry->spec.start/sizeof(unsigned int);
        subparcel->vegetationGroups[0].count = subparcel->vegetationIndicesEntry->spec.count/sizeof(unsigned int);
        subparcel->vegetationGroups[0].materialIndex = 0;
      }
      {
        // std::cout << "march objects ax " << (unsigned int)(void *)geometrySet << " " << (unsigned int)(void *)marchObjects << " " << numMarchObjects << std::endl;

        unsigned int numPositions;
        unsigned int numUvs;
        unsigned int numAtlasUvs;
        unsigned int numIds;
        unsigned int numIndices;
        unsigned int numSkyLights;
        unsigned int numTorchLights;
        doGetMarchThingStats(geometrySet, subparcel.get(), numPositions, numUvs, numAtlasUvs, numIds, numIndices, numSkyLights, numTorchLights);

        ArenaAllocator *positionsAllocator = tracker->thingPositionsAllocator;
        ArenaAllocator *uvsAllocator = tracker->thingUvsAllocator;
        ArenaAllocator *atlasUvsAllocator = tracker->thingAtlasUvsAllocator;
        ArenaAllocator *idsAllocator = tracker->thingIdsAllocator;
        ArenaAllocator *indicesAllocator = tracker->thingIndicesAllocator;
        ArenaAllocator *skyLightsAllocator = tracker->thingSkyLightsAllocator;
        ArenaAllocator *torchLightsAllocator = tracker->thingTorchLightsAllocator;

        subparcel->thingPositionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
        if (!subparcel->thingPositionsEntry) {
          std::cout << "could not allocate chunk matchThings positions" << std::endl;
          abort();
        }
        subparcel->thingUvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
        if (!subparcel->thingUvsEntry) {
          std::cout << "could not allocate chunk matchThings uvs" << std::endl;
          abort();
        }
        subparcel->thingAtlasUvsEntry = uvsAllocator->alloc(numAtlasUvs*sizeof(float));
        if (!subparcel->thingAtlasUvsEntry) {
          std::cout << "could not allocate chunk matchThings uvs" << std::endl;
          abort();
        }
        subparcel->thingIdsEntry = idsAllocator->alloc(numIds*sizeof(float));
        if (!subparcel->thingIdsEntry) {
          std::cout << "could not allocate chunk matchThings ids" << std::endl;
          abort();
        }
        subparcel->thingIndicesEntry = indicesAllocator->alloc(numIndices*sizeof(unsigned int));
        if (!subparcel->thingIndicesEntry) {
          std::cout << "could not allocate chunk matchThings indices" << std::endl;
          abort();
        }
        subparcel->thingSkyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
        if (!subparcel->thingSkyLightsEntry) {
          std::cout << "could not allocate chunk matchThings skyLights" << std::endl;
          abort();
        }
        subparcel->thingTorchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
        if (!subparcel->thingTorchLightsEntry) {
          std::cout << "could not allocate chunk matchThings torchLights" << std::endl;
          abort();
        }

        float *positions = (float *)(positionsAllocator->data + subparcel->thingPositionsEntry->spec.start);
        float *uvs = (float *)(uvsAllocator->data + subparcel->thingUvsEntry->spec.start);
        float *atlasUvs = (float *)(atlasUvsAllocator->data + subparcel->thingAtlasUvsEntry->spec.start);
        float *ids = (float *)(idsAllocator->data + subparcel->thingIdsEntry->spec.start);
        unsigned int *indices = (unsigned int *)(indicesAllocator->data + subparcel->thingIndicesEntry->spec.start);
        unsigned char *skyLights = (unsigned char *)(skyLightsAllocator->data + subparcel->thingSkyLightsEntry->spec.start);
        unsigned char *torchLights = (unsigned char *)(torchLightsAllocator->data + subparcel->thingTorchLightsEntry->spec.start);

        /* slab => (slab.position.byteOffset - geometry.attributes.position.array.byteOffset)/Float32Array.BYTES_PER_ELEMENT;
        const indexOffset = vegetationMesh.getSlabPositionOffset(slab)/3;
        for (let i = 0; i < spec.indices.length; i++) {
          spec.indices[i] += indexOffset;
        } */
        Subparcel *subparcels = nullptr;
        unsigned int numSubparcels = 0;
        unsigned int indexOffset = subparcel->vegetationPositionsEntry->spec.start/sizeof(float)/3;
        doMarchThings(tracker, geometrySet, subparcel->coord.x, subparcel->coord.y, subparcel->coord.z, subparcel.get(), subparcels, numSubparcels, positions, uvs, atlasUvs, ids, indices, skyLights, torchLights, indexOffset, textureUpdated);

        // groups
        subparcel->thingGroups[0].start = subparcel->thingIndicesEntry->spec.start/sizeof(unsigned int);
        subparcel->thingGroups[0].count = subparcel->thingIndicesEntry->spec.count/sizeof(unsigned int);
        subparcel->thingGroups[0].materialIndex = 0;
      }
      doLandPhysics(tracker, subparcel.get(), landPositions, numLandPositions);
      doObjectPhysics(tracker, geometrySet, subparcel.get());
      doThingPhysics(tracker, geometrySet, subparcel.get());

      // output
      // std::cout << "return 1" << std::endl;
      Message->method = (int)MESSAGES::updateGeometry;

      // std::cout << "return 2" << std::endl;

      index = 0;
      *((FreeEntry **)(Message->args + index)) = subparcel->landPositionsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landNormalsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landUvsEntry.get();
      index += sizeof(FreeEntry *);
      /* *((FreeEntry **)(Message->args + index)) = subparcel->landBarycentricsEntry.get();
      index += sizeof(FreeEntry *); */
      *((FreeEntry **)(Message->args + index)) = subparcel->landAosEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landIdsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landSkyLightsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landTorchLightsEntry.get();
      index += sizeof(FreeEntry *);

      // std::cout << "return 3" << std::endl;

      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationPositionsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationUvsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIdsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIndicesEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationSkyLightsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationTorchLightsEntry.get();
      index += sizeof(FreeEntry *);

      *((FreeEntry **)(Message->args + index)) = subparcel->thingPositionsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->thingUvsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->thingAtlasUvsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->thingIdsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->thingIndicesEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->thingSkyLightsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->thingTorchLightsEntry.get();
      index += sizeof(FreeEntry *);

      if (textureUpdated) {
        *((unsigned char **)(Message->args + index)) = tracker->atlasTexture.data();
        index += sizeof(unsigned char *);
      } else {
        *((unsigned char **)(Message->args + index)) = nullptr;
        index += sizeof(unsigned char *);
      }

      *((std::shared_ptr<Subparcel> **)(Message->args + index)) = subparcelSharedPtr;
      index += sizeof(std::shared_ptr<Subparcel> *);

      // std::cout << "return 4" << std::endl;

      if (index/sizeof(unsigned int) > Message->count) {
        std::cout << "message overflow " << index << " " << Message->count << std::endl;
        abort();
      }

      threadPool->outbox.push(Message);

      // std::cout << "return update " << (void *)subparcelSharedPtr << " " << subparcel->coord.x << " " << subparcel->coord.y << " " << subparcel->coord.z << std::endl;
    }
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // mine
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float delta = *((float *)(Message->args + index));
    index += sizeof(float);

    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doMine(tracker, position, delta);
    const bool &doable = spec.first;
    if (doable) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->landPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landNormalsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landAosEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      std::vector<int> indices;
      {
        int x = (int)std::floor(position[0]);
        int y = (int)std::floor(position[1]);
        int z = (int)std::floor(position[2]);
        constexpr int radius = 1;
        for (int dy = -radius - 1; dy <= radius + 1; dy++) {
          const int ay = y + dy;
          for (int dz = -radius - 1; dz <= radius + 1; dz++) {
            const int az = z + dz;
            for (int dx = -radius - 1; dx <= radius + 1; dx++) {
              const int ax = x + dx;
              const int sx = (int)std::floor((float)ax/(float)SUBPARCEL_SIZE);
              const int sy = (int)std::floor((float)ay/(float)SUBPARCEL_SIZE);
              const int sz = (int)std::floor((float)az/(float)SUBPARCEL_SIZE);
              const int index = getSubparcelIndex(sx, sy, sz);
              auto iter = std::find(indices.begin(), indices.end(), index);
              if (iter == indices.end()) {
                indices.push_back(index);
              }
            }
          }
        }
      }
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // releaseMine
    unsigned int index = 0;

    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);

    unsigned int numSubparcels = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    {
      std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
      for (unsigned int i = 0; i < numSubparcels; i++) {
        std::shared_ptr<Subparcel> *subparcelSharedPtr = *((std::shared_ptr<Subparcel> **)(Message->args + index));
        index += sizeof(std::shared_ptr<Subparcel> *);

        if ((*subparcelSharedPtr)->live) {
          tracker->subparcels[(*subparcelSharedPtr)->coord.index] = *subparcelSharedPtr;
        }
        delete subparcelSharedPtr;
      }
    }

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // light
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float delta = *((float *)(Message->args + index));
    index += sizeof(float);

    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doLight(tracker, position, delta);
    const bool &doable = spec.first;
    if (doable) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->landPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landNormalsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landAosEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      std::vector<int> indices;
      {
        int x = (int)std::floor(position[0]);
        int y = (int)std::floor(position[1]);
        int z = (int)std::floor(position[2]);
        constexpr int radius = 1;
        for (int dy = -radius - 1; dy <= radius + 1; dy++) {
          const int ay = y + dy;
          for (int dz = -radius - 1; dz <= radius + 1; dz++) {
            const int az = z + dz;
            for (int dx = -radius - 1; dx <= radius + 1; dx++) {
              const int ax = x + dx;
              const int sx = (int)std::floor((float)ax/(float)SUBPARCEL_SIZE);
              const int sy = (int)std::floor((float)ay/(float)SUBPARCEL_SIZE);
              const int sz = (int)std::floor((float)az/(float)SUBPARCEL_SIZE);
              const int index = getSubparcelIndex(sx, sy, sz);
              auto iter = std::find(indices.begin(), indices.end(), index);
              if (iter == indices.end()) {
                indices.push_back(index);
              }
            }
          }
        }
      }
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }

    /* unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float delta = *((float *)(Message->args + index));
    index += sizeof(float);

    std::vector<std::shared_ptr<Subparcel>> mineSpecs = doLight(tracker, position, delta); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // addObject
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *name = (char *)(Message->args + index);
    index += MAX_NAME_LENGTH;
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float *quaternion = (float *)(Message->args + index);
    index += 4*sizeof(float);

    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doAddObject(tracker, geometrySet, OBJECT_TYPE::VEGETATION, name, position, quaternion);
    if (spec.first) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      for (const std::shared_ptr<Subparcel> &subparcel : newSubparcels) {
        doObjectPhysics(tracker, geometrySet, subparcel.get());
      }

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIndicesEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      const int sx = (int)std::floor(position[0]/(float)SUBPARCEL_SIZE);
      const int sy = (int)std::floor(position[1]/(float)SUBPARCEL_SIZE);
      const int sz = (int)std::floor(position[2]/(float)SUBPARCEL_SIZE);
      const int subparcelIndex = getSubparcelIndex(sx, sy, sz);
      std::vector<int> indices{subparcelIndex};
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // removeObject
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    int sx = *((int *)(Message->args + index));
    index += sizeof(int);
    int sy = *((int *)(Message->args + index));
    index += sizeof(int);
    int sz = *((int *)(Message->args + index));
    index += sizeof(int);
    unsigned int objectId = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    const int subparcelIndex = getSubparcelIndex(sx, sy, sz);
    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doRemoveObject(tracker, geometrySet, subparcelIndex, objectId);
    if (spec.first) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      for (const std::shared_ptr<Subparcel> &subparcel : newSubparcels) {
        doObjectPhysics(tracker, geometrySet, subparcel.get());
      }

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIndicesEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      std::vector<int> indices{subparcelIndex};
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // releaseAddRemoveObject
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    unsigned int numSubparcels = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    {
      std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
      for (unsigned int i = 0; i < numSubparcels; i++) {
        std::shared_ptr<Subparcel> *subparcelSharedPtr = *((std::shared_ptr<Subparcel> **)(Message->args + index));
        index += sizeof(std::shared_ptr<Subparcel> *);

        if ((*subparcelSharedPtr)->live) {
          tracker->subparcels[(*subparcelSharedPtr)->coord.index] = *subparcelSharedPtr;
        }
        delete subparcelSharedPtr;
      }
    }

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // addThingGeometry
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *nameCharPtr = (char *)(Message->args + index);
    index += MAX_NAME_LENGTH;
    float *positions = *((float **)(Message->args + index));
    index += sizeof(float *);
    float *uvs = *((float **)(Message->args + index));
    index += sizeof(float *);
    unsigned int *indices = *((unsigned int **)(Message->args + index));
    index += sizeof(unsigned int *);
    unsigned int numPositions = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    unsigned int numUvs = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    unsigned int numIndices = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    unsigned char *texture = *((unsigned char **)(Message->args + index));
    index += sizeof(unsigned char *);
    PhysicsGeometry *trianglePhysicsGeometry = *((PhysicsGeometry **)(Message->args + index));
    index += sizeof(PhysicsGeometry *);

    Geometry *geometry = new Geometry();
    geometry->name = std::string(nameCharPtr);
    geometry->positions.resize(numPositions);
    memcpy(geometry->positions.data(), positions, numPositions * sizeof(float));
    geometry->uvs.resize(numUvs);
    memcpy(geometry->uvs.data(), uvs, numUvs * sizeof(float));
    geometry->indices.resize(numIndices);
    memcpy(geometry->indices.data(), indices, numIndices * sizeof(float));
    geometry->aabb.setFromPositions(positions, numPositions);
    geometry->texture = texture;

    /* PxDefaultMemoryOutputStream *writeStream = doBakeGeometry(&tracker->physicer, positions, indices, numPositions, numIndices);
    std::pair<PxTriangleMesh *, PxTriangleMeshGeometry *> spec = doMakeBakedGeometryRaw(&tracker->physicer, writeStream); // XXX GC the TriangleMesh
    geometry->physxGeometry = spec.second; */
    geometry->physxGeometry = trianglePhysicsGeometry->geometry;

    geometrySet->thingGeometries.push_back(geometry);
    geometrySet->geometryMap[geometry->name] = geometry;

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // addThing
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *name = (char *)(Message->args + index);
    index += MAX_NAME_LENGTH;
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float *quaternion = (float *)(Message->args + index);
    index += 4*sizeof(float *);

    bool textureUpdated;
    std::tuple<bool, std::vector<std::shared_ptr<Subparcel>>, bool> spec = doAddThing(tracker, geometrySet, name, position, quaternion);
    if (std::get<0>(spec)) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = std::get<1>(spec);
      bool textureUpdated = std::get<2>(spec);

      for (const std::shared_ptr<Subparcel> &subparcel : newSubparcels) {
        doThingPhysics(tracker, geometrySet, subparcel.get());
      }

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->thingPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->thingUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->thingAtlasUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->thingIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->thingIndicesEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->thingSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->thingTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      if (textureUpdated) {
        *((unsigned char **)(Message->args + index)) = tracker->atlasTexture.data();
        index += sizeof(unsigned char *);
      } else {
        *((unsigned char **)(Message->args + index)) = nullptr;
        index += sizeof(unsigned char *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      const int sx = (int)std::floor(position[0]/(float)SUBPARCEL_SIZE);
      const int sy = (int)std::floor(position[1]/(float)SUBPARCEL_SIZE);
      const int sz = (int)std::floor(position[2]/(float)SUBPARCEL_SIZE);
      const int subparcelIndex = getSubparcelIndex(sx, sy, sz);
      std::vector<int> indices{subparcelIndex};
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }
  },
};

}