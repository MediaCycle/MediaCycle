// ActionScript file

package {
	import flash.display.Sprite;
	import flash.events.NetStatusEvent;
	import flash.media.Microphone;
	import flash.media.Video;
	import flash.net.NetConnection;
	import flash.net.NetStream;

	public class NetConnectionExample extends Sprite {
		//private var videoURL:String="test.flv";
		private var connection:NetConnection = new NetConnection();
		private var stream:NetStream;
		private var video:Video = new Video();
		private var out_ns:NetStream;
		private var inStream:NetStream;
		private var microphone:Microphone = Microphone.getMicrophone(-1);
		private var outStream:NetStream;
			

		public function NetConnectionExample():void {
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
		
		public function audioRecord(soundFileRecord:String):void {
			outStream  = new NetStream(connection);
			microphone.setUseEchoSuppression(true);
			microphone.setLoopBack(true);
       
			microphone.rate = 44;
            microphone.gain = 70;
                
			outStream.attachAudio(microphone);
			soundFileRecord = soundFileRecord.substring(0,(soundFileRecord.length)-4);
			//soundFileRecord = "mp3:test.mp3";
			//trace(soundFileRecord);
			outStream.publish(soundFileRecord,"record");
			
		}
		
		public function stopAudioRecord():void{
			outStream.close();
		}
		
		public function audioPlay(soundFilePlay:String):void{
			inStream = new NetStream(connection);
			inStream.play(soundFilePlay);
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

