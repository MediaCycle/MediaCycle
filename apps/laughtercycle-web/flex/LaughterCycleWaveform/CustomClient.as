package
{
	public class CustomClient
	{
		public var duration:Number;
		public var frameRate:Number;
		
		public function CustomClient()
		{
			duration = 0;
		}
		
		public function onMetaData(info:Object):void{
			duration = info.duration;
			frameRate = info.audiodatarate;
			trace(duration);
			//trace(info.duration);
		}
		public function onPlayStatus():void{
			//trace("ok ok");
		}

	}
}