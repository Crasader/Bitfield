LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
                   $(LOCAL_PATH)/../../../Classes/PlayerData/Player.cpp \
                   $(LOCAL_PATH)/../../../Classes/Util.cpp \
                   $(LOCAL_PATH)/../../../Classes/Input.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameObject/Ship.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameObject/Bit.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameObject/Carrier.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameObject/Barrier.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameObject/Blossom.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameObject/Snake.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameObject/SquadronFactory.cpp \
                   $(LOCAL_PATH)/../../../Classes/Scene/SplashScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/Scene/GameScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/HUD.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/World.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/Bits/Generator.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/Bits/UpgradeItem.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/Bits/BitsPanel.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/Fleet/SquadronPanel.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/Fleet/FleetPanel.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/Fleet/SquadronSlot.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/Fleet/SquadronCard.cpp \
                   $(LOCAL_PATH)/../../../Classes/UI/PurchaseButton.cpp



LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
					$(LOCAL_PATH)/../../../Classes/GameObject \
					$(LOCAL_PATH)/../../../Classes/Scene \
					$(LOCAL_PATH)/../../../Classes/UI

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END

APP_PLATFORM := android-14