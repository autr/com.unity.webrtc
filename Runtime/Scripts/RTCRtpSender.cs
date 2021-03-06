using System;
using System.Runtime.InteropServices;

namespace Unity.WebRTC
{   
    public class RTCRtpSender
    {
        internal IntPtr self;
        private RTCPeerConnection peer;

        public RTCRtpSender()
        {
        }

        internal RTCRtpSender(IntPtr ptr, RTCPeerConnection peer)
        {
            self = ptr;
            this.peer = peer;
        }

        public RTCStatsReportAsyncOperation GetStats()
        {
            return peer.GetStats(this);
        }

        public MediaStreamTrack Track
        {
            get
            {
                IntPtr ptr = NativeMethods.SenderGetTrack(self);
                return WebRTC.FindOrCreate<MediaStreamTrack>(ptr, _ptr => new MediaStreamTrack(_ptr));
            }
        }

        public RTCRtpSendParameters GetParameters()
        {
            NativeMethods.SenderGetParameters(self, out var ptr);
            RTCRtpSendParametersInternal parametersInternal = Marshal.PtrToStructure<RTCRtpSendParametersInternal>(ptr);
            RTCRtpSendParameters parameters = new RTCRtpSendParameters(parametersInternal);
            Marshal.FreeHGlobal(ptr);
            return parameters;
        }

        public RTCErrorType SetParameters(RTCRtpSendParameters parameters)
        {
            IntPtr ptr = parameters.CreatePtr();
            RTCErrorType error = NativeMethods.SenderSetParameters(self, ptr);
            RTCRtpSendParameters.DeletePtr(ptr);

            return error;
        }
        public void SetHardwareParameters(RTCRtpEncodingParametersInternal parameters)
        {
            IntPtr ptr = Marshal.AllocCoTaskMem(Marshal.SizeOf(parameters));
            Marshal.StructureToPtr(parameters, ptr, false);
            NativeMethods.SetHardwareParameters(ptr);
        }
    }
}
