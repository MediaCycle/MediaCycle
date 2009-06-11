package {
    import flash.display.Sprite;
    import flash.events.*;
    import flash.media.Microphone;
    import flash.system.Security;

	// Utiliser NetConnection et NetStream pour transmettre l'audio au serveur
    public class MicrophoneExample extends Sprite {
        public function MicrophoneExample() {
            var mic:Microphone = Microphone.getMicrophone();
            Security.showSettings("2");
            mic.setLoopBack(true);
           if (mic != null) {
                mic.setUseEchoSuppression(true);
                mic.addEventListener(ActivityEvent.ACTIVITY, activityHandler);
                mic.addEventListener(StatusEvent.STATUS, statusHandler);
                
            }
        }
        
        private function activityHandler(event:ActivityEvent):void {
            trace("activityHandler: " + event);
        }

        private function statusHandler(event:StatusEvent):void {
            trace("statusHandler: " + event);
        }
    }
}