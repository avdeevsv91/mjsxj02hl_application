#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "./video.h"
#include "./../localsdk.h"
#include "./../../logger/logger.h"
#include "./../../configs/configs.h"
#include "./../../rtsp/rtsp.h"
#include "./../../rtsp/libRtspServer.h"

// Video capture callback
int h26x_capture_callback(int chn, LOCALSDK_H26X_FRAME_INFO *frameInfo) {
    if(frameInfo && frameInfo->size) {
        if(rtsp_media_frame(chn, frameInfo->data, frameInfo->size, frameInfo->timestamp, frameInfo->type)) {
            return LOCALSDK_OK;
        }
    }
    return LOCALSDK_ERROR;
}

int h26x_capture_primary_channel(LOCALSDK_H26X_FRAME_INFO *frameInfo) {
    return h26x_capture_callback(LOCALSDK_VIDEO_PRIMARY_CHANNEL, frameInfo);
}

int h26x_capture_secondary_channel(LOCALSDK_H26X_FRAME_INFO *frameInfo) {
    return h26x_capture_callback(LOCALSDK_VIDEO_SECONDARY_CHANNEL, frameInfo);
}

// Init video
bool video_init() {
    logger("video", "video_init", LOGGER_LEVEL_DEBUG, "Function is called...");
    if(local_sdk_video_init(APP_CFG.video.fps) == LOCALSDK_OK) {
        logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_init()");
        int changed_resolution_type;
        LOCALSDK_PICTURE_SIZE picture_size;
        // Init channel 0
        if(inner_change_resulu_type(LOCALSDK_VIDEO_RESOLUTION_1920x1080, &changed_resolution_type) == LOCALSDK_OK) {
            logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "inner_change_resulu_type(LOCALSDK_VIDEO_RESOLUTION_1920x1080)");
            if(SAMPLE_COMM_SYS_GetPicSize(changed_resolution_type, &picture_size) == LOCALSDK_OK) {
                logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "SAMPLE_COMM_SYS_GetPicSize(LOCALSDK_VIDEO_RESOLUTION_1920x1080)");
                LOCALSDK_VIDEO_OPTIONS video_options = {
                    .bitrate     = 8 * LOCALSDK_VIDEO_PRIMARY_BITRATE,
                    .fps         = APP_CFG.video.fps,
                    .resolution  = LOCALSDK_VIDEO_RESOLUTION_1920x1080,
                    .flip        = APP_CFG.video.flip,
                    .mirror      = APP_CFG.video.mirror,
                    .unknown_5   = 0, // FIXME: what is it?
                    .video       = true,
                    .unknown_7   = 1, // FIXME: what is it?
                    .payload     = APP_CFG.video.type,
                    .rcmode      = LOCALSDK_VIDEO_RCMODE_TYPE,
                    .gop         = 1 * APP_CFG.video.fps,
                    .screen_size = picture_size.width * picture_size.height,
                    .unknown_12  = 327680, // FIXME: what is it?
                    .jpeg        = false,
                    .unknown_14  = 0, // FIXME: what is it?
                };
                if(local_sdk_video_create(LOCALSDK_VIDEO_PRIMARY_CHANNEL, &video_options) == LOCALSDK_OK) {
                    logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_create(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                    if(local_sdk_video_set_parameters(LOCALSDK_VIDEO_PRIMARY_CHANNEL, &video_options) == LOCALSDK_OK) {
                        logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_set_parameters(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                        LOCALSDK_OSD_OPTIONS osd_options = {
                            .unknown_0 = 67, // FIXME: what is it?
                            .unknown_1 = 48, // FIXME: what is it?
                            .unknown_2 = 0, // FIXME: what is it?
                            .unknown_3 = 1, // FIXME: what is it?
                            .unknown_4 = 1, // FIXME: what is it?
                            .unknown_5 = 2, // FIXME: what is it?
                            .unknown_6 = 0, // FIXME: what is it?
                            .unknown_7 = 1, // FIXME: what is it?
                            .unknown_8 = 1, // FIXME: what is it?
                        };
                        if(local_sdk_video_osd_set_parameters(LOCALSDK_VIDEO_PRIMARY_CHANNEL, &osd_options) == LOCALSDK_OK) {
                            logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_osd_set_parameters(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                            if(local_sdk_video_set_encode_frame_callback(LOCALSDK_VIDEO_PRIMARY_CHANNEL, h26x_capture_primary_channel) == LOCALSDK_OK) {
                                logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_set_encode_frame_callback(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                                if(local_sdk_video_start(LOCALSDK_VIDEO_PRIMARY_CHANNEL) == LOCALSDK_OK) {
                                    logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_start(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                                    if(local_sdk_video_run(LOCALSDK_VIDEO_PRIMARY_CHANNEL) == LOCALSDK_OK) {
                                        logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_run(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                                        // Init channel 1
                                        if(inner_change_resulu_type(LOCALSDK_VIDEO_RESOLUTION_640x360, &changed_resolution_type) == LOCALSDK_OK) {
                                            logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "inner_change_resulu_type(LOCALSDK_VIDEO_RESOLUTION_640x360)");
                                            if(SAMPLE_COMM_SYS_GetPicSize(changed_resolution_type, &picture_size) == LOCALSDK_OK) {
                                                logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "SAMPLE_COMM_SYS_GetPicSize(LOCALSDK_VIDEO_RESOLUTION_640x360)");
                                                video_options.bitrate     = LOCALSDK_VIDEO_SECONDARY_BITRATE;
                                                video_options.resolution  = LOCALSDK_VIDEO_RESOLUTION_640x360;
                                                video_options.unknown_5   = 1; // FIXME: what is it?
                                                video_options.unknown_7   = 0; // FIXME: what is it?
                                                video_options.screen_size = picture_size.width * picture_size.height;
                                                video_options.unknown_12  = 50000; // FIXME: what is it?
                                                video_options.jpeg        = true;
                                                video_options.unknown_14  = 1; // FIXME: what is it?
                                                if(local_sdk_video_create(LOCALSDK_VIDEO_SECONDARY_CHANNEL, &video_options) == LOCALSDK_OK) {
                                                    logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_create(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                    if(local_sdk_video_set_parameters(LOCALSDK_VIDEO_SECONDARY_CHANNEL, &video_options) == LOCALSDK_OK) {
                                                        logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_set_parameters(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                        if(local_sdk_video_set_encode_frame_callback(LOCALSDK_VIDEO_SECONDARY_CHANNEL, h26x_capture_secondary_channel) == LOCALSDK_OK) {
                                                            logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_set_encode_frame_callback(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                            if(local_sdk_video_set_algo_module_register_callback(local_sdk_alarm_algo_module_register_callback) == LOCALSDK_OK) {
                                                                logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_set_algo_module_register_callback()");
                                                                if(local_sdk_video_start(LOCALSDK_VIDEO_SECONDARY_CHANNEL) == LOCALSDK_OK) {
                                                                    logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_start(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                                    if(local_sdk_video_run(LOCALSDK_VIDEO_SECONDARY_CHANNEL) == LOCALSDK_OK) {
                                                                        logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_run(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                                        
                                                                        logger("video", "video_init", LOGGER_LEVEL_DEBUG, "Function completed.");
                                                                        return true;
                                                                    } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_run(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                                } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_start(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                            } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_set_algo_module_register_callback()");
                                                        } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_set_encode_frame_callback(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                    } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_set_parameters(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                                } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_create(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
                                            } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "SAMPLE_COMM_SYS_GetPicSize(LOCALSDK_VIDEO_RESOLUTION_640x360)");
                                        } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "inner_change_resulu_type(LOCALSDK_VIDEO_RESOLUTION_640x360)");
                                    } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_run(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                                } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_start(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                            } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_set_encode_frame_callback(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                        } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_osd_set_parameters(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
                    } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_set_parameters(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");

                } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_create(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
            } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "SAMPLE_COMM_SYS_GetPicSize(LOCALSDK_VIDEO_RESOLUTION_1920x1080)");
        } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "inner_change_resulu_type(LOCALSDK_VIDEO_RESOLUTION_1920x1080)");
    } else logger("video", "video_init", LOGGER_LEVEL_ERROR, "%s error!", "local_sdk_video_init()");
    if(video_free()) {
        logger("video", "video_init", LOGGER_LEVEL_INFO, "%s success.", "video_free()");
    } else logger("video", "video_init", LOGGER_LEVEL_WARNING, "%s error!", "video_free()");
    logger("video", "video_init", LOGGER_LEVEL_DEBUG, "Function completed.");
    return false;
}

// Free video
bool video_free() {
    bool result = true;
    logger("video", "video_free", LOGGER_LEVEL_DEBUG, "Function is called...");
    
    // Stop secondary video
    if(local_sdk_video_stop(LOCALSDK_VIDEO_SECONDARY_CHANNEL, true) == LOCALSDK_OK) {
        logger("video", "video_free", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_stop(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
    } else {
        logger("video", "video_free", LOGGER_LEVEL_WARNING, "%s error!", "local_sdk_video_stop(LOCALSDK_VIDEO_SECONDARY_CHANNEL)");
        result = false;
    }
    
    // Stop primary video
    if(local_sdk_video_stop(LOCALSDK_VIDEO_PRIMARY_CHANNEL, true) == LOCALSDK_OK) {
        logger("video", "video_free", LOGGER_LEVEL_INFO, "%s success.", "local_sdk_video_stop(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
    } else {
        logger("video", "video_free", LOGGER_LEVEL_WARNING, "%s error!", "local_sdk_video_stop(LOCALSDK_VIDEO_PRIMARY_CHANNEL)");
        result = false;
    }

    logger("video", "video_free", LOGGER_LEVEL_DEBUG, "Function completed.");
    return result;
}
