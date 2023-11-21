#include "cSoundUtils.h"
#include <iostream>

void CheckFMODError(FMOD_RESULT result, const char* file, int line) {
	if (result != FMOD_OK) {
		std::cerr << "FMOD Error in " << file << " (Line " << line << "): " << GetFMODErrorString(result) << " : " << result << std::endl;
		// You can also add more error handling logic here if needed.
	}
}

const char* GetFMODErrorString(FMOD_RESULT result) {
	switch (result)
	{
	case FMOD_ERR_BADCOMMAND:
		return "FMOD_ERR_BADCOMMAND";
		break;
	case FMOD_ERR_CHANNEL_ALLOC:
		return "FMOD_ERR_CHANNEL_ALLOC";
		break;
	case FMOD_ERR_CHANNEL_STOLEN:
		return "FMOD_ERR_CHANNEL_STOLEN";
		break;
	case FMOD_ERR_DMA:
		return "FMOD_ERR_DMA";
		break;
	case FMOD_ERR_DSP_CONNECTION:
		return "FMOD_ERR_DSP_CONNECTION";
		break;
	case FMOD_ERR_DSP_DONTPROCESS:
		return "FMOD_ERR_DSP_DONTPROCESS";
		break;
	case FMOD_ERR_DSP_FORMAT:
		return "FMOD_ERR_DSP_FORMAT";
		break;
	case FMOD_ERR_DSP_INUSE:
		return "FMOD_ERR_DSP_INUSE";
		break;
	case FMOD_ERR_DSP_NOTFOUND:
		return "FMOD_ERR_DSP_NOTFOUND";
		break;
	case FMOD_ERR_DSP_RESERVED:
		return "FMOD_ERR_DSP_RESERVED";
		break;
	case FMOD_ERR_DSP_SILENCE:
		return "FMOD_ERR_DSP_SILENCE";
		break;
	case FMOD_ERR_DSP_TYPE:
		return "FMOD_ERR_DSP_TYPE";
		break;
	case FMOD_ERR_FILE_BAD:
		return "FMOD_ERR_FILE_BAD";
		break;
	case FMOD_ERR_FILE_COULDNOTSEEK:
		return "FMOD_ERR_FILE_COULDNOTSEEK";
		break;
	case FMOD_ERR_FILE_DISKEJECTED:
		return "FMOD_ERR_FILE_DISKEJECTED";
		break;
	case FMOD_ERR_FILE_EOF:
		return "FMOD_ERR_FILE_EOF";
		break;
	case FMOD_ERR_FILE_ENDOFDATA:
		return "FMOD_ERR_FILE_ENDOFDATA";
		break;
	case FMOD_ERR_FILE_NOTFOUND:
		return "FMOD_ERR_FILE_NOTFOUND";
		break;
	case FMOD_ERR_FORMAT:
		return "FMOD_ERR_FORMAT";
		break;
	case FMOD_ERR_HEADER_MISMATCH:
		return "FMOD_ERR_HEADER_MISMATCH";
		break;
	case FMOD_ERR_HTTP:
		return "FMOD_ERR_HTTP";
		break;
	case FMOD_ERR_HTTP_ACCESS:
		return "FMOD_ERR_HTTP_ACCESS";
		break;
	case FMOD_ERR_HTTP_PROXY_AUTH:
		return "FMOD_ERR_HTTP_PROXY_AUTH";
		break;
	case FMOD_ERR_HTTP_SERVER_ERROR:
		return "FMOD_ERR_HTTP_SERVER_ERROR";
		break;
	case FMOD_ERR_HTTP_TIMEOUT:
		return "FMOD_ERR_HTTP_TIMEOUT";
		break;
	case FMOD_ERR_INITIALIZATION:
		return "FMOD_ERR_INITIALIZATION";
		break;
	case FMOD_ERR_INITIALIZED:
		return "FMOD_ERR_INITIALIZED";
		break;
	case FMOD_ERR_INTERNAL:
		return "FMOD_ERR_INTERNAL";
		break;
	case FMOD_ERR_INVALID_FLOAT:
		return "FMOD_ERR_INVALID_FLOAT";
		break;
	case FMOD_ERR_INVALID_HANDLE:
		return "FMOD_ERR_INVALID_HANDLE";
		break;
	case FMOD_ERR_INVALID_PARAM:
		return "FMOD_ERR_INVALID_PARAM";
		break;
	case FMOD_ERR_INVALID_POSITION:
		return "FMOD_ERR_INVALID_POSITION";
		break;
	case FMOD_ERR_INVALID_SPEAKER:
		return "FMOD_ERR_INVALID_SPEAKER";
		break;
	case FMOD_ERR_INVALID_SYNCPOINT:
		return "FMOD_ERR_INVALID_SYNCPOINT";
		break;
	case FMOD_ERR_INVALID_THREAD:
		return "FMOD_ERR_INVALID_THREAD";
		break;
	case FMOD_ERR_INVALID_VECTOR:
		return "FMOD_ERR_INVALID_VECTOR";
		break;
	case FMOD_ERR_MAXAUDIBLE:
		return "FMOD_ERR_MAXAUDIBLE";
		break;
	case FMOD_ERR_MEMORY:
		return "FMOD_ERR_MEMORY";
		break;
	case FMOD_ERR_MEMORY_CANTPOINT:
		return "FMOD_ERR_MEMORY_CANTPOINT";
		break;
	case FMOD_ERR_NEEDS3D:
		return "FMOD_ERR_NEEDS3D";
		break;
	case FMOD_ERR_NEEDSHARDWARE:
		return "FMOD_ERR_NEEDSHARDWARE";
		break;
	case FMOD_ERR_NET_CONNECT:
		return "FMOD_ERR_NET_CONNECT";
		break;
	case FMOD_ERR_NET_SOCKET_ERROR:
		return "FMOD_ERR_NET_SOCKET_ERROR";
		break;
	case FMOD_ERR_NET_URL:
		return "FMOD_ERR_NET_URL";
		break;
	case FMOD_ERR_NET_WOULD_BLOCK:
		return "FMOD_ERR_NET_WOULD_BLOCK";
		break;
	case FMOD_ERR_NOTREADY:
		return "FMOD_ERR_NOTREADY";
		break;
	case FMOD_ERR_OUTPUT_ALLOCATED:
		return "FMOD_ERR_OUTPUT_ALLOCATED";
		break;
	case FMOD_ERR_OUTPUT_CREATEBUFFER:
		return "FMOD_ERR_OUTPUT_CREATEBUFFER";
		break;
	case FMOD_ERR_OUTPUT_DRIVERCALL:
		return "FMOD_ERR_OUTPUT_DRIVERCALL";
		break;
	case FMOD_ERR_OUTPUT_FORMAT:
		return "FMOD_ERR_OUTPUT_FORMAT";
		break;
	case FMOD_ERR_OUTPUT_INIT:
		return "FMOD_ERR_OUTPUT_INIT";
		break;
	case FMOD_ERR_OUTPUT_NODRIVERS:
		return "FMOD_ERR_OUTPUT_NODRIVERS";
		break;
	case FMOD_ERR_PLUGIN:
		return "FMOD_ERR_PLUGIN";
		break;
	case FMOD_ERR_PLUGIN_MISSING:
		return "FMOD_ERR_PLUGIN_MISSING";
		break;
	case FMOD_ERR_PLUGIN_RESOURCE:
		return "FMOD_ERR_PLUGIN_RESOURCE";
		break;
	case FMOD_ERR_PLUGIN_VERSION:
		return "FMOD_ERR_PLUGIN_VERSION";
		break;
	case FMOD_ERR_RECORD:
		return "FMOD_ERR_RECORD";
		break;
	case FMOD_ERR_REVERB_CHANNELGROUP:
		return "FMOD_ERR_REVERB_CHANNELGROUP";
		break;
	case FMOD_ERR_REVERB_INSTANCE:
		return "FMOD_ERR_REVERB_INSTANCE";
		break;
	case FMOD_ERR_SUBSOUNDS:
		return "FMOD_ERR_SUBSOUNDS";
		break;
	case FMOD_ERR_SUBSOUND_ALLOCATED:
		return "FMOD_ERR_SUBSOUND_ALLOCATED";
		break;
	case FMOD_ERR_SUBSOUND_CANTMOVE:
		return "FMOD_ERR_SUBSOUND_CANTMOVE";
		break;
	case FMOD_ERR_TAGNOTFOUND:
		return "FMOD_ERR_TAGNOTFOUND";
		break;
	case FMOD_ERR_TOOMANYCHANNELS:
		return "FMOD_ERR_TOOMANYCHANNELS";
		break;
	case FMOD_ERR_TRUNCATED:
		return "FMOD_ERR_TRUNCATED";
		break;
	case FMOD_ERR_UNIMPLEMENTED:
		return "FMOD_ERR_UNIMPLEMENTED";
		break;
	case FMOD_ERR_UNINITIALIZED:
		return "FMOD_ERR_UNINITIALIZED";
		break;
	case FMOD_ERR_UNSUPPORTED:
		return "FMOD_ERR_UNSUPPORTED";
		break;
	case FMOD_ERR_VERSION:
		return "FMOD_ERR_VERSION";
		break;
	case FMOD_ERR_EVENT_ALREADY_LOADED:
		return "FMOD_ERR_EVENT_ALREADY_LOADED";
		break;
	case FMOD_ERR_EVENT_LIVEUPDATE_BUSY:
		return "FMOD_ERR_EVENT_LIVEUPDATE_BUSY";
		break;
	case FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH:
		return "FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH";
		break;
	case FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT:
		return "FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT";
		break;
	case FMOD_ERR_EVENT_NOTFOUND:
		return "FMOD_ERR_EVENT_NOTFOUND";
		break;
	case FMOD_ERR_STUDIO_UNINITIALIZED:
		return "FMOD_ERR_STUDIO_UNINITIALIZED";
		break;
	case FMOD_ERR_STUDIO_NOT_LOADED:
		return "FMOD_ERR_STUDIO_NOT_LOADED";
		break;
	case FMOD_ERR_INVALID_STRING:
		return "FMOD_ERR_INVALID_STRING";
		break;
	case FMOD_ERR_ALREADY_LOCKED:
		return "FMOD_ERR_ALREADY_LOCKED";
		break;
	case FMOD_ERR_NOT_LOCKED:
		return "FMOD_ERR_NOT_LOCKED";
		break;
	case FMOD_ERR_RECORD_DISCONNECTED:
		return "FMOD_ERR_RECORD_DISCONNECTED";
		break;
	case FMOD_ERR_TOOMANYSAMPLES:
		return "FMOD_ERR_TOOMANYSAMPLES";
		break;
	case FMOD_RESULT_FORCEINT:
		return "FMOD_RESULT_FORCEINT";
		break;
	default:
		return "Unknown Error";
		break;
	}
}