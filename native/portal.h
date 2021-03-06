#ifndef PORTAL_H
#define PORTAL_H

#include <legion.h>
using namespace Legion;
using namespace LegionRuntime::Arrays;

enum TaskID {
  TASK_WRAPPER_ID,
  TOP_LEVEL_TASK_WRAPPER_ID,
};

static inline CoherenceProperty getCoherenceProperty(jint jcoherence)
{
  switch (static_cast<int>(jcoherence)) {
    case 0:
      return EXCLUSIVE;
    case 1:
      return ATOMIC;
    case 2:
      return SIMULTANEOUS;
    case 3:
      return RELAXED;
    default:
      assert(0);
  }
}

static inline PrivilegeMode getPrivilegeMode(jint jpriv)
{
  switch (static_cast<int>(jpriv)) {
    case 0:
      return NO_ACCESS;
    case 1:
      return READ_ONLY;
    case 2:
      return READ_WRITE;
    case 3:
      return WRITE_ONLY;
    case 4:
      return WRITE_DISCARD;
    case 5:
      return REDUCE;
    default:
      assert(0);
  }
}

struct PointWithRank {
  PointWithRank(unsigned rank) :
    rank(rank)
  {}
  unsigned rank;
};

template<unsigned DIM>
struct PointWithRankImpl : public PointWithRank {
  PointWithRankImpl(int *coord) :
    PointWithRank(DIM), point((coord_t*)coord)
  {}
  Point<DIM> point;
};

struct RectWithRank {
  RectWithRank(unsigned rank) :
    rank(rank)
  {}
  unsigned rank;
};

template<unsigned DIM>
struct RectWithRankImpl : public RectWithRank {
  RectWithRankImpl(Point<DIM> low, Point<DIM> high) :
    RectWithRank(DIM), rect(low, high)
  {}
  Rect<DIM> rect;
};

/*
 *
 */
template<class PTR, class DERIVED> class LegionNativeClass {
 public:
  static jclass getJClass(JNIEnv *env, const char *jclazz_name) {
    jclass jclazz = env->FindClass(jclazz_name);
    assert(jclazz != NULL);
    return jclazz;
  }

  static jfieldID getHandleFieldID(JNIEnv *env) {
    static jfieldID fid = env->GetFieldID(
        DERIVED::getJClass(env), "nativeHandle", "J");
    assert(fid != NULL);
    return fid;
  }

  static void setHandle(JNIEnv *env, jobject jobj, PTR ptr) {
    env->SetLongField(jobj, getHandleFieldID(env), reinterpret_cast<jlong>(ptr));
  }
};

/*
 * Virtualizes the task_id
 */
class TaskLauncherWrapper {
 public:

  TaskLauncherWrapper() {
    launcher.task_id = TASK_WRAPPER_ID;
  }

  TaskLauncher launcher;

  int task_id;
  size_t arg_size;
  char *arg_data;
};

struct IndexLauncherWrapper {
  IndexLauncher launcher;
  int task_id;
  size_t arg_size;
  char *arg_data;
};

struct TaskArgumentWrapper {
  int task_id;
  size_t size;
  char data[];
};


/*
 *
 */
class TaskLauncherJni : public LegionNativeClass<TaskLauncherWrapper*, TaskLauncherJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/TaskLauncher");
  }
};

/*
 *
 */
class FutureJni : public LegionNativeClass<Future*, FutureJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/Future");
  }
};

class IndexSpaceJni : public LegionNativeClass<IndexSpace*, IndexSpaceJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/IndexSpace");
  }
};

class FieldSpaceJni : public LegionNativeClass<FieldSpace*, FieldSpaceJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/FieldSpace");
  }
};

class FieldAllocatorJni : public LegionNativeClass<FieldAllocator*, FieldAllocatorJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/FieldAllocator");
  }
};

class LogicalRegionJni : public LegionNativeClass<LogicalRegion*, LogicalRegionJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/LogicalRegion");
  }
};

class RegionRequirementJni : public LegionNativeClass<RegionRequirement*, RegionRequirementJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/RegionRequirement");
  }
};

class InlineLauncherJni : public LegionNativeClass<InlineLauncher*, InlineLauncherJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/InlineLauncher");
  }
};

class IndexAllocatorJni : public LegionNativeClass<IndexAllocator*, IndexAllocatorJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/IndexAllocator");
  }
};

class PointJni : public LegionNativeClass<PointWithRank*, PointJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/Point");
  }
};

class RectJni : public LegionNativeClass<RectWithRank*, RectJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/Rect");
  }
};

class DomainJni : public LegionNativeClass<Domain*, DomainJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/Domain");
  }
};

class DomainColoringJni : public LegionNativeClass<DomainColoring*, DomainColoringJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/DomainColoring");
  }
};

class IndexPartitionJni : public LegionNativeClass<IndexPartition*, IndexPartitionJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/IndexPartition");
  }
};

class LogicalPartitionJni : public LegionNativeClass<LogicalPartition*, LogicalPartitionJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/LogicalPartition");
  }
};

class ArgumentMapJni : public LegionNativeClass<ArgumentMap*, ArgumentMapJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/ArgumentMap");
  }
};

class IndexLauncherJni : public LegionNativeClass<IndexLauncherWrapper*, IndexLauncherJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/IndexLauncher");
  }
};

class FutureMapJni : public LegionNativeClass<FutureMap*, FutureMapJni> {
 public:
  static jclass getJClass(JNIEnv *env) {
    return LegionNativeClass::getJClass(env, "org/legion/FutureMap");
  }
};

//class XJni : public LegionNativeClass<X*, XJni> {
// public:
//  static jclass getJClass(JNIEnv *env) {
//    return LegionNativeClass::getJClass(env, "org/legion/X");
//  }
//};

#endif
