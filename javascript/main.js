var audioContext = null;
var meter = null;
var canvasContext = null;
var WIDTH=1000;
var HEIGHT=100;
var RESOLUTION=500;
var rafID = null;

var goButton = document.getElementById('goButton');

goButton.onclick = function() {

	canvasContext = document.getElementById( "meter" ).getContext("2d");
    window.AudioContext = window.AudioContext || window.webkitAudioContext;
    audioContext = new AudioContext();

    try {
        navigator.getUserMedia =
        	navigator.getUserMedia ||
        	navigator.webkitGetUserMedia ||
        	navigator.mozGetUserMedia;

        navigator.getUserMedia(
        {
            "audio": {
                "mandatory": {
                    "googEchoCancellation": "false",
                    "googAutoGainControl": "false",
                    "googNoiseSuppression": "false",
                    "googHighpassFilter": "false"
                },
                "optional": []
            },
        }, gotStream, didntGetStream);
    } catch (e) {
        alert('getUserMedia threw exception :' + e);
    }

}


function didntGetStream() {
    alert('Stream generation failed.');
}

var mediaStreamSource = null;

function gotStream(stream) {
    mediaStreamSource = audioContext.createMediaStreamSource(stream);
    meter = createAudioMeter(audioContext);
    mediaStreamSource.connect(meter);
    drawLoop();
}

function drawLoop( time ) {
    canvasContext.clearRect(0,0,WIDTH,HEIGHT);
    canvasContext.fillStyle = "green";
    var w = WIDTH/meter.maxVols;
    var x,y,h;
    for (var i=0; i<meter.volumes.length; i++){
        x = i*w;
        y = 0.5*(1-meter.volumes[i])*HEIGHT;
        h = meter.volumes[i]*HEIGHT;
        canvasContext.fillRect(x,y,w,h);
    }
    rafID = window.requestAnimationFrame( drawLoop );
}

function createAudioMeter(audioContext) {
	var processor = audioContext.createScriptProcessor(512);
	processor.onaudioprocess = volumeAudioProcess;
	processor.volumes = [0];
    processor.maxVols = RESOLUTION;
	processor.connect(audioContext.destination);

	processor.shutdown =
		function(){
			this.disconnect();
			this.onaudioprocess = null;
		};

	return processor;
}

function volumeAudioProcess( event ) {
	var buf = event.inputBuffer.getChannelData(0);
    var bufLength = buf.length;
	var sum = 0;

    var x;
    for (var i=0; i<bufLength; i++) {
    	x = buf[i];
    	sum += x * x;
    }
    var rms =  Math.sqrt(sum / bufLength);

    this.volumes.unshift(rms);
    if (this.volumes.length>this.maxVols){
        this.volumes.pop();
    }
}
