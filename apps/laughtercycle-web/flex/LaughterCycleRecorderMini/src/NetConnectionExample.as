// ActionScript file

package {
	import flash.display.Sprite;
	import flash.events.NetStatusEvent;
	import flash.media.Microphone;
	import flash.media.Video;
	import flash.net.NetConnection;
	import flash.net.NetStream;
	import flash.net.*;
	import flash.events.*;
	import mx.rpc.http.*;
	import mx.rpc.events.*;
	
	

	public class NetConnectionExample extends Sprite {
		//private var videoURL:String="test.flv";
		private var connection:NetConnection = new NetConnection();
		private var stream:NetStream;
		private var video:Video = new Video();
		private var out_ns:NetStream;
		private var inStream:NetStream;
		private var microphone:Microphone = Microphone.getMicrophone(-1);
		private var outStream:NetStream;
		//private var serverIP:String = "192.168.1.119";//should be in a separate config file on the server
		private var serverIP:String;
		private var HTTP:HTTPService;

		public function NetConnectionExample():void {
			HTTP=new HTTPService();
			HTTP.method="GET";
			HTTP.url="serverip.php";
			HTTP.resultFormat="text";
			//This is to return the php results
			HTTP.addEventListener(ResultEvent.RESULT,redConnexion);
			HTTP.send();			
		}
		
		public function redConnexion(event:ResultEvent):void {
        		serverIP = event.result.toString();
        		//testText.text = event.result.toString();
        		connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
			//connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			connection.connect("rtmp://" + serverIP + "/oflaDemo");
			connection.client = {'onBWDone':function(event:Event):void{trace('onBWDone');}};
        		//netX = new NetConnectionExample(serverIP);
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

