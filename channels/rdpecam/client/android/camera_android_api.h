/* SPDX-FileCopyrightText: 2026 Daniel Campos Ramos
 * SPDX-License-Identifier: Apache-2.0 */
#pragma once

/* Camera2 and ImageReader start at API 24. Keep them out of DT_NEEDED so an
 * API-23 embedding can still load the ordinary receiver. Resolve every entry
 * before registering the HAL; keep the libraries loaded for callback lifetime. */
#include <dlfcn.h>
#include <string.h>
#include <winpr/synch.h>

static struct
{
	__typeof__(&ACameraCaptureSession_close) ACameraCaptureSession_close;
	__typeof__(&ACameraCaptureSession_setRepeatingRequest) ACameraCaptureSession_setRepeatingRequest;
	__typeof__(&ACameraCaptureSession_stopRepeating) ACameraCaptureSession_stopRepeating;
	__typeof__(&ACameraDevice_close) ACameraDevice_close;
	__typeof__(&ACameraDevice_createCaptureRequest) ACameraDevice_createCaptureRequest;
	__typeof__(&ACameraDevice_createCaptureSession) ACameraDevice_createCaptureSession;
	__typeof__(&ACameraManager_create) ACameraManager_create;
	__typeof__(&ACameraManager_delete) ACameraManager_delete;
	__typeof__(&ACameraManager_deleteCameraIdList) ACameraManager_deleteCameraIdList;
	__typeof__(&ACameraManager_getCameraCharacteristics) ACameraManager_getCameraCharacteristics;
	__typeof__(&ACameraManager_getCameraIdList) ACameraManager_getCameraIdList;
	__typeof__(&ACameraManager_openCamera) ACameraManager_openCamera;
	__typeof__(&ACameraMetadata_free) ACameraMetadata_free;
	__typeof__(&ACameraMetadata_getConstEntry) ACameraMetadata_getConstEntry;
	__typeof__(&ACameraOutputTarget_create) ACameraOutputTarget_create;
	__typeof__(&ACameraOutputTarget_free) ACameraOutputTarget_free;
	__typeof__(&ACaptureRequest_addTarget) ACaptureRequest_addTarget;
	__typeof__(&ACaptureRequest_free) ACaptureRequest_free;
	__typeof__(&ACaptureRequest_removeTarget) ACaptureRequest_removeTarget;
	__typeof__(&ACaptureSessionOutputContainer_add) ACaptureSessionOutputContainer_add;
	__typeof__(&ACaptureSessionOutputContainer_create) ACaptureSessionOutputContainer_create;
	__typeof__(&ACaptureSessionOutputContainer_free) ACaptureSessionOutputContainer_free;
	__typeof__(&ACaptureSessionOutput_create) ACaptureSessionOutput_create;
	__typeof__(&ACaptureSessionOutput_free) ACaptureSessionOutput_free;
	__typeof__(&AImageReader_acquireLatestImage) AImageReader_acquireLatestImage;
	__typeof__(&AImageReader_delete) AImageReader_delete;
	__typeof__(&AImageReader_getWindow) AImageReader_getWindow;
	__typeof__(&AImageReader_new) AImageReader_new;
	__typeof__(&AImageReader_setImageListener) AImageReader_setImageListener;
	__typeof__(&AImage_delete) AImage_delete;
	__typeof__(&AImage_getHeight) AImage_getHeight;
	__typeof__(&AImage_getPlaneData) AImage_getPlaneData;
	__typeof__(&AImage_getPlanePixelStride) AImage_getPlanePixelStride;
	__typeof__(&AImage_getPlaneRowStride) AImage_getPlaneRowStride;
	__typeof__(&AImage_getWidth) AImage_getWidth;
} cam_android_api;

static INIT_ONCE cam_android_api_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK cam_android_load_api(WINPR_ATTR_UNUSED PINIT_ONCE once,
	WINPR_ATTR_UNUSED PVOID parameter, WINPR_ATTR_UNUSED PVOID* context)
{
	if (__builtin_available(android 24, *))
	{
		/* Resolve the API-24 functions below; no eager symbol references. */
	}
	else
	{
		WLog_ERR(TAG, "Camera redirection requires Android API 24 or newer");
		return FALSE;
	}

	void* camera = dlopen("libcamera2ndk.so", RTLD_NOW | RTLD_LOCAL);
	void* media = dlopen("libmediandk.so", RTLD_NOW | RTLD_LOCAL);
	if (!camera || !media)
	{
		WLog_ERR(TAG, "Camera2 or ImageReader library unavailable");
		goto fail;
	}

#define RESOLVE_CAMERA_API(library, name) \
	do \
	{ \
		cam_android_api.name = (__typeof__(cam_android_api.name))dlsym(library, #name); \
		if (!cam_android_api.name) \
		{ \
			WLog_ERR(TAG, "Required camera API %s unavailable", #name); \
			goto fail; \
		} \
	} while (0)

	RESOLVE_CAMERA_API(camera, ACameraCaptureSession_close);
	RESOLVE_CAMERA_API(camera, ACameraCaptureSession_setRepeatingRequest);
	RESOLVE_CAMERA_API(camera, ACameraCaptureSession_stopRepeating);
	RESOLVE_CAMERA_API(camera, ACameraDevice_close);
	RESOLVE_CAMERA_API(camera, ACameraDevice_createCaptureRequest);
	RESOLVE_CAMERA_API(camera, ACameraDevice_createCaptureSession);
	RESOLVE_CAMERA_API(camera, ACameraManager_create);
	RESOLVE_CAMERA_API(camera, ACameraManager_delete);
	RESOLVE_CAMERA_API(camera, ACameraManager_deleteCameraIdList);
	RESOLVE_CAMERA_API(camera, ACameraManager_getCameraCharacteristics);
	RESOLVE_CAMERA_API(camera, ACameraManager_getCameraIdList);
	RESOLVE_CAMERA_API(camera, ACameraManager_openCamera);
	RESOLVE_CAMERA_API(camera, ACameraMetadata_free);
	RESOLVE_CAMERA_API(camera, ACameraMetadata_getConstEntry);
	RESOLVE_CAMERA_API(camera, ACameraOutputTarget_create);
	RESOLVE_CAMERA_API(camera, ACameraOutputTarget_free);
	RESOLVE_CAMERA_API(camera, ACaptureRequest_addTarget);
	RESOLVE_CAMERA_API(camera, ACaptureRequest_free);
	RESOLVE_CAMERA_API(camera, ACaptureRequest_removeTarget);
	RESOLVE_CAMERA_API(camera, ACaptureSessionOutputContainer_add);
	RESOLVE_CAMERA_API(camera, ACaptureSessionOutputContainer_create);
	RESOLVE_CAMERA_API(camera, ACaptureSessionOutputContainer_free);
	RESOLVE_CAMERA_API(camera, ACaptureSessionOutput_create);
	RESOLVE_CAMERA_API(camera, ACaptureSessionOutput_free);
	RESOLVE_CAMERA_API(media, AImageReader_acquireLatestImage);
	RESOLVE_CAMERA_API(media, AImageReader_delete);
	RESOLVE_CAMERA_API(media, AImageReader_getWindow);
	RESOLVE_CAMERA_API(media, AImageReader_new);
	RESOLVE_CAMERA_API(media, AImageReader_setImageListener);
	RESOLVE_CAMERA_API(media, AImage_delete);
	RESOLVE_CAMERA_API(media, AImage_getHeight);
	RESOLVE_CAMERA_API(media, AImage_getPlaneData);
	RESOLVE_CAMERA_API(media, AImage_getPlanePixelStride);
	RESOLVE_CAMERA_API(media, AImage_getPlaneRowStride);
	RESOLVE_CAMERA_API(media, AImage_getWidth);
#undef RESOLVE_CAMERA_API
	return TRUE;

fail:
	if (camera)
		dlclose(camera);
	if (media)
		dlclose(media);
	memset(&cam_android_api, 0, sizeof(cam_android_api));
	return FALSE;
}

#define ACameraCaptureSession_close cam_android_api.ACameraCaptureSession_close
#define ACameraCaptureSession_setRepeatingRequest cam_android_api.ACameraCaptureSession_setRepeatingRequest
#define ACameraCaptureSession_stopRepeating cam_android_api.ACameraCaptureSession_stopRepeating
#define ACameraDevice_close cam_android_api.ACameraDevice_close
#define ACameraDevice_createCaptureRequest cam_android_api.ACameraDevice_createCaptureRequest
#define ACameraDevice_createCaptureSession cam_android_api.ACameraDevice_createCaptureSession
#define ACameraManager_create cam_android_api.ACameraManager_create
#define ACameraManager_delete cam_android_api.ACameraManager_delete
#define ACameraManager_deleteCameraIdList cam_android_api.ACameraManager_deleteCameraIdList
#define ACameraManager_getCameraCharacteristics cam_android_api.ACameraManager_getCameraCharacteristics
#define ACameraManager_getCameraIdList cam_android_api.ACameraManager_getCameraIdList
#define ACameraManager_openCamera cam_android_api.ACameraManager_openCamera
#define ACameraMetadata_free cam_android_api.ACameraMetadata_free
#define ACameraMetadata_getConstEntry cam_android_api.ACameraMetadata_getConstEntry
#define ACameraOutputTarget_create cam_android_api.ACameraOutputTarget_create
#define ACameraOutputTarget_free cam_android_api.ACameraOutputTarget_free
#define ACaptureRequest_addTarget cam_android_api.ACaptureRequest_addTarget
#define ACaptureRequest_free cam_android_api.ACaptureRequest_free
#define ACaptureRequest_removeTarget cam_android_api.ACaptureRequest_removeTarget
#define ACaptureSessionOutputContainer_add cam_android_api.ACaptureSessionOutputContainer_add
#define ACaptureSessionOutputContainer_create cam_android_api.ACaptureSessionOutputContainer_create
#define ACaptureSessionOutputContainer_free cam_android_api.ACaptureSessionOutputContainer_free
#define ACaptureSessionOutput_create cam_android_api.ACaptureSessionOutput_create
#define ACaptureSessionOutput_free cam_android_api.ACaptureSessionOutput_free
#define AImageReader_acquireLatestImage cam_android_api.AImageReader_acquireLatestImage
#define AImageReader_delete cam_android_api.AImageReader_delete
#define AImageReader_getWindow cam_android_api.AImageReader_getWindow
#define AImageReader_new cam_android_api.AImageReader_new
#define AImageReader_setImageListener cam_android_api.AImageReader_setImageListener
#define AImage_delete cam_android_api.AImage_delete
#define AImage_getHeight cam_android_api.AImage_getHeight
#define AImage_getPlaneData cam_android_api.AImage_getPlaneData
#define AImage_getPlanePixelStride cam_android_api.AImage_getPlanePixelStride
#define AImage_getPlaneRowStride cam_android_api.AImage_getPlaneRowStride
#define AImage_getWidth cam_android_api.AImage_getWidth
