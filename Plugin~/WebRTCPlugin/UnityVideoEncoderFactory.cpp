#include "pch.h"
#include "UnityVideoEncoderFactory.h"

#include "DummyVideoEncoder.h"

#if defined(__APPLE__)
#import "sdk/objc/components/video_codec/RTCDefaultVideoEncoderFactory.h"
#endif

namespace unity
{    
namespace webrtc
{

    bool IsFormatSupported(
        const std::vector<webrtc::SdpVideoFormat>& supported_formats,
        const webrtc::SdpVideoFormat& format)
    {
        for (const webrtc::SdpVideoFormat& supported_format : supported_formats)
        {
            if (cricket::IsSameCodec(format.name, format.parameters,
                supported_format.name,
                supported_format.parameters))
            {
                return true;
            }
        }
        return false;
    }

    webrtc::VideoEncoderFactory* GetDefaultEncoderFactory()
    {
#if defined(__APPLE__)
        return (webrtc::VideoEncoderFactory*)[[RTCDefaultVideoEncoderFactory alloc] init];
#endif
        return new webrtc::InternalEncoderFactory();
    }

    UnityVideoEncoderFactory::UnityVideoEncoderFactory(IVideoEncoderObserver* observer)
    : internal_encoder_factory_(GetDefaultEncoderFactory())

    {
        m_observer = observer;
    }
    
    UnityVideoEncoderFactory::~UnityVideoEncoderFactory()
    {
#if defined(__APPLE__)
        RTCDefaultVideoEncoderFactory* factory =
            (RTCDefaultVideoEncoderFactory*)internal_encoder_factory_.release();
        [factory release];
#endif
    }


    std::vector<webrtc::SdpVideoFormat> UnityVideoEncoderFactory::GetHardwareEncoderFormats() const
    {
        return { webrtc::CreateH264Format(webrtc::H264::kProfileConstrainedBaseline, webrtc::H264::kLevel5_1, "1") };
    }


    std::vector<webrtc::SdpVideoFormat> UnityVideoEncoderFactory::GetSupportedFormats() const
    {
        std::vector <webrtc::SdpVideoFormat> formats = GetHardwareEncoderFormats();

        // todo(kazuki): should support codec other than h264 like vp8, vp9 and av1.
        // 
        // std::vector <webrtc::SdpVideoFormat> formats2 = internal_encoder_factory_->GetSupportedFormats();
        // formats.insert(formats.end(), formats2.begin(), formats2.end());
        
        return formats;
    }

    webrtc::VideoEncoderFactory::CodecInfo UnityVideoEncoderFactory::QueryVideoEncoder(const webrtc::SdpVideoFormat& format) const
    {
        if (IsFormatSupported(GetHardwareEncoderFormats(), format))
        {
            return CodecInfo{ true, false };
        }
        RTC_DCHECK(IsFormatSupported(GetSupportedFormats(), format));
        return internal_encoder_factory_->QueryVideoEncoder(format);
    }

    std::unique_ptr<webrtc::VideoEncoder> UnityVideoEncoderFactory::CreateVideoEncoder(const webrtc::SdpVideoFormat& format)
    {
        if (IsFormatSupported(GetHardwareEncoderFormats(), format))
        {
            return std::make_unique<DummyVideoEncoder>(m_observer);
        }

        std::unique_ptr<webrtc::VideoEncoder> internalEncoder;
        // Try creating internal encoder.
        if (IsFormatSupported(GetSupportedFormats(), format))
        {
            internalEncoder = internal_encoder_factory_->CreateVideoEncoder(format);
        }
        return internalEncoder;
    }

}
}
