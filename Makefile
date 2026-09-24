#---------------------------------------------------------------------------------------------------------------------
# Sanguine GBA — see third_party/butano/template/Makefile for the full variable reference.
#---------------------------------------------------------------------------------------------------------------------
TARGET      	:=  sanguine_gba
BUILD       	:=  build
LIBBUTANO   	:=  third_party/butano/butano
PYTHON      	:=  python3
SOURCES     	:=  source source/engine source/rooms source/ui source/dialogue
INCLUDES    	:=  source third_party/butano/common/include
DATA        	:=
GRAPHICS    	:=  assets/rooms assets/sprites third_party/butano/common/graphics
AUDIO       	:=  assets/audio
AUDIOBACKEND	:=  maxmod
AUDIOTOOL		:=
DMGAUDIO    	:=  assets/dmg_audio
DMGAUDIOBACKEND	:=  default
ROMTITLE    	:=  SANGUINE
ROMCODE     	:=  SNGN
USERFLAGS   	:=
USERCXXFLAGS	:=
USERASFLAGS 	:=
USERLDFLAGS 	:=
USERLIBDIRS 	:=
USERLIBS    	:=
DEFAULTLIBS 	:=
STACKTRACE		:=
USERBUILD   	:=
EXTTOOL     	:=

#---------------------------------------------------------------------------------------------------------------------
# Export absolute butano path:
#---------------------------------------------------------------------------------------------------------------------
ifndef LIBBUTANOABS
	export LIBBUTANOABS	:=	$(realpath $(LIBBUTANO))
endif

#---------------------------------------------------------------------------------------------------------------------
# Include main makefile:
#---------------------------------------------------------------------------------------------------------------------
include $(LIBBUTANOABS)/butano.mak
