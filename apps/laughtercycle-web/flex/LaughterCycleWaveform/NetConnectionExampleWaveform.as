// ActionScript file

package {
	import flash.display.Sprite;
	import flash.events.*;
	import flash.media.Microphone;
	import flash.media.Video;
	import flash.net.NetConnection;
	import flash.net.NetStream;

	public class NetConnectionExampleWaveform extends Sprite {
		//private var videoURL:String="test.flv";
		private var connection:NetConnection = new NetConnection();
		private var stream:NetStream;
		private var video:Video = new Video();
		private var out_ns:NetStream;
		private var inStream:NetStream;
		private var microphone:Microphone = Microphone.getMicrophone(-1);
		private var outStream:NetStream;
		public var playTime:Number;
			

		public function NetConnectionExampleWaveform():void {
			connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			//connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			connection.connect("rtmp://localhost/oflaDemo");
			connection.client = {'onBWDone':function(event){trace('onBWDone');}};			
		}

		private function netStatusHandler(event:NetStatusEvent):void {
			switch (event.info.code) {
				case "NetConnection.Connect.Success" :
					connectStream();
					break;
				case "NetStream.Play.StreamNotFound" :
					trace("Stream not found");
					break;
			}
		}
		
		public function audioRecord():void {
			outStream  = new NetStream(connection);
			microphone.setUseEchoSuppression(true);
			microphone.setLoopBack(true);
       
			microphone.rate = 44;
            microphone.gain = 70;
                
			outStream.attachAudio(microphone);
			trace("Micro attaché");
			outStream.publish("test","record");
			trace("Normalement ça enregistre");
			trace(microphone.activityLevel);
			
		}
		
		public function stopAudioRecord():void{
			outStream.close();
		}
		
		public function audioPlay(soundFile:String):void{
			inStream = new NetStream(connection);
			inStream.addEventListener(NetStatusEvent.NET_STATUS,endAudioPlay);
			inStream.addEventListener(Event.ENTER_FRAME,updateSlider);
			inStream.play(soundFile);
			
		}
		
		public function endAudioPlay(event:NetStatusEvent):void{
			if(event.info.code == "NetStream.Play.Stop"){
				inStream.close();
				inStream.removeEventListener(NetStatusEvent.NET_STATUS,updateSlider);
			}
		}
		
		public function updateSlider(event:Event):void{
			playTime = inStream.time;
		}

		private function connectStream():void {
			var stream:NetStream=new NetStream(connection);

			stream.client = new CustomClient();
			this.video.attachNetStream(stream);
			//stream.play(videoURL);
			//addChild(video);
		}
		
	}
}

class CustomClient {

	public function onBWDone():void {
		trace("yes");
	}
}

