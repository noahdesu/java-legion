package org.legion;

/**
 *
 */
public class Pointer extends LegionObject {

  Pointer(long handle) {
    nativeHandle = handle;
  }

  @Override
  protected void disposeInternal() {
    assert(isInitialized());
    disposeInternal(nativeHandle);
  }

  private native void disposeInternal(long handle);
}
