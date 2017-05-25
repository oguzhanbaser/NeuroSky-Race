// Colour settings
if(themeColour == 'white'){
	var metric = '#a9a9a9';
	var backColor = '#7d7d7d';
	var pointerColor = '#898989'; 	
	var pageBackgorund = '#fff';
	var pieTrack = metric;
	var pieBar = backColor;
	var gaugeTrackColor = metric;
	var gaugeBarColor = backColor;
	var gaugePointerColor = '#ccc';
	var pieSegColors = [metric,'#868686','#636363','#404040','#1d1d1d'];	
}
else {
	//default to black
	var backColor = '#4f4f4f';
	var metric = '#f2f2f2';	
	var pointerColor = '#898989'; 
	var pageBackgorund = '#2b2b2b';	
	var pieSegColors = [metric,'#c0c0c0','#8e8e8e','#5b5b5b','#292929'];
	var pieTrack = backColor;
	var pieBar = metric;
	var gaugeTrackColor = '#4f4f4f';
	var gaugeBarColor = '#898989';
	var gaugePointerColor = metric;
}

// Stores
var cf_rSVPs = [];
var cf_rGs = [];
var cf_rLs = [];
var cf_rPs = [];
var cf_rRags = [];
var cf_rFunnels = [];

$(document).ready(function(){

	var socket = io('http://'+ window.location.hostname + ':3000');

    //windows resize
	cfSizeItems();
    readyGauge();
    readySparkLine();

    var gId1 = "cf-gauge-1";
    var gId2 = "cf-gauge-2";

    
    var gChart1 = $("#spark-1");
    var gChart2 = $("#spark-2");

    var connStatus1 = 0;
    var connStatus2 = 0;
	var val1, val2;

	socket.on('player1', function(p_data){
		val1 = p_data["attention"];
		connStatus1 = p_data["signal"];
		console.log(connStatus1);
	});

	socket.on('player2', function(p_data){
		val2 = p_data["attention"];
		connStatus2 = p_data["signal"];
	});

    if(connStatus1 == 100)
    {
        $("#conn-img-1").attr("src", "./img/green.png");
    }else{
        $("#conn-img-1").attr("src", "./img/red.png");
	}

    if(connStatus2 == 100)
    {
        $("#conn-img-2").attr("src", "./img/green.png");
    }else{
        $("#conn-img-1").attr("src", "./img/red.png");
	}

    var myData1 = [];
    var myData2 = [];

    setInterval(function(){

		/*
        var val1 = Math.floor((Math.random() * 100) + 1);
        var val2 = Math.floor((Math.random() * 100) + 1);
		var connStatus1 = 100;
		var connStatus2 = 100;*/

        $("#qVal1").text(connStatus1);
        $("#qVal2").text(connStatus2);

        if(connStatus1 != 100)
        	$("#qVal1").css("color", "red");
        else 
        	$("#qVal1").css("color", "green");

        if(connStatus2 != 100)
        	$("#qVal2").css("color", "red");
        else 
        	$("#qVal2").css("color", "green");

        gaugeUpdate(gId1, {'newVal': val1});
        gaugeUpdate(gId2, {'newVal': val2});

        myData1.push(val1);
        myData2.push(val2);

        if(myData1.length > 10)
            myData1.shift();

        if(myData2.length > 10)
            myData2.shift();

        createSparkline(gChart1, myData1, window.cf_defaultSparkOpts);
        createSparkline(gChart2, myData2, window.cf_defaultSparkOpts);

		if(connStatus1 == 100)
		{
			$("#conn-img-1").attr("src", "./img/green.png");
		}else{
			$("#conn-img-1").attr("src", "./img/red.png");
		}

		if(connStatus2 == 100)
		{
			$("#conn-img-2").attr("src", "./img/green.png");
		}else{
			$("#conn-img-2").attr("src", "./img/red.png");
		}
    }, 1000);

    updateOpts1 = {'newVal': "10"};

    updateOpts2 = {'newVal': "50"};
    gaugeUpdate(gId2, updateOpts2);

});


function cfSizeItems(){
	var width = $(window).width();

	$('.cf-item').each(function(){
		if(width > 767 ){
			$(this).height($(this).width());
		}
		else{
			$(this).height('auto');
		}
	});
}

$(window).resize(function(){
	cfSizeItems();
});


window.cf_defaultSparkOpts = {};
function readySparkLine()
{
	cf_defaultSparkOpts.fillColor = false;
	cf_defaultSparkOpts.lineColor = metric;
	cf_defaultSparkOpts.lineWidth = 1.5;
	cf_defaultSparkOpts.minSpotColor = false;
	cf_defaultSparkOpts.maxSpotColor = false;
	cf_defaultSparkOpts.spotRadius = 2.5;
	cf_defaultSparkOpts.highlightLineColor = metric;
	cf_defaultSparkOpts.spotColor = '#f8f77d';
	
	// Initialise sparklines
	/*
	*	Copy the each() function for each sparkline you have
	* 	e.g. $('#spark-1').each(function(){.....}
	*/	
	$('.sparkline').each(function(){
		
		/*
		// Set custom options and merge with default
		customSparkOptions = {};
		customSparkOptions.minSpotColor = true;
		var sparkOptions = cf_defaultSparkOpts;
		var sparkOptions = $.extend({}, cf_defaultSparkOpts, customSparkOptions);
		*/
		
		// No custom options
		var sparkOptions = cf_defaultSparkOpts;
			
		data = 	[2343,1765,2000,2453,2122,2333,2666,3000,2654,2322,2500,2700,2654,2456,2892,3292];
		//createSparkline($(this), data, sparkOptions);
	});	
}

function readyGauge()
{
    $('.cf-gauge').each(function(){

		// Gather IDs 
		var gId = $(this).prop('id');					// Gauge container id e.g. cf-gauge-1
		var gcId = $('canvas', $(this)).prop('id');		// Gauge canvas id e.g. cf-gauge-1-g
		var gmId = $('.metric', $(this)).prop('id');  	// Gauge metric id e.g. cf-gauge-1-m

		//Create a canvas
		var ratio = 2.1;
		var width = $('.canvas',$(this)).width();
		var height = Math.round(width/ratio);
		$('canvas', $(this)).prop('width', width).prop('height', height);

		// Set options  	
		rGopts = {};
		rGopts.lineWidth = 0.30;
		rGopts.strokeColor = gaugeTrackColor;
		rGopts.limitMax = true;
		rGopts.colorStart = gaugeBarColor;
		rGopts.percentColors = void 0;	
		rGopts.pointer = {
			length: 0.7,
			strokeWidth: 0.035,
			color: gaugePointerColor
		};

		// Create gauge
		cf_rGs[gId] = new Gauge(document.getElementById(gcId)).setOptions(rGopts);
		cf_rGs[gId].setTextField(document.getElementById(gmId));

		// Set up values for gauge (in reality it'll likely be done one by one calling the function, not from here)
        
        updateOpts = {'minVal':'0','maxVal':'100','newVal': "0"};
        gaugeUpdate(gId, updateOpts);
        
        /*
		var val = 0
		setInterval(function(){
			val += 50;
			if(val >= 1000) val = 0;
			updateOpts = {'minVal':'0','maxVal':'1000','newVal': val};
			gaugeUpdate(gId, updateOpts);
		}, 1000);*/


		// Responsiveness	
		$(window).resize(function(){
		
			//Get canvas measurements
			var ratio = 2.1;
			var width = $('.canvas', $('#'+gId)).width();
			var height = Math.round(width/ratio);

			cf_rGs[gId].ctx.clearRect(0, 0, width, height);
			$('canvas', $('#'+gId)).width(width).height(height);
			cf_rGs[gId].render();
		});

	});
}

function createSparkline(obj, data, sparkOptions){
	
	$(window).resize(generateSparkline);
	
	function generateSparkline(){
		var ww = $(window).width();
		var $obj = obj;			
		var $parent = $obj.parent().parent();
	
		// Current value
		$('.sparkline-value .metric-small', $parent).html(data[data.length-1]);
	
		// Sizing
		if(ww < 768){
			var cWidth = $parent.width();
			var slWidth = Math.floor(cWidth/3);
		}
		else{
			var svWidth = $('.sparkline-value', $parent).width();
			var cWidth = $parent.width();
			var slWidth = cWidth - svWidth - 20;
			var cHeight = $parent.parent().outerHeight() - 35;
			var svmHeight = $('.cf-svmc', $parent).height();
			var slHeight = cHeight - svmHeight - 300;
			$('.sparkline-value', $parent).css({height:slHeight});
		}	
	
		// Options
		sparkOptions.width = slWidth;
		sparkOptions.height = slHeight;		
	
		// Create sparkline
		$obj.sparkline(data, sparkOptions);
	}
	
	// Call once on page load
	generateSparkline();
}

function gaugeUpdate(gauge, opts){
	if(opts.minVal){
		$('.val-min .metric-small', $('#'+gauge)).html(opts.minVal);		
		cf_rGs[gauge].minValue = opts.minVal;
	}
	if(opts.maxVal){
		cf_rGs[gauge].maxValue = opts.maxVal;
		$('.val-max .metric-small', $('#'+gauge)).html(opts.maxVal);
	}
	if(opts.newVal){
        rGopts = {};
        if(opts.newVal < 33)
        {
            rGopts.strokeColor = "#1E740A";
            rGopts.colorStart = "#33D010";
            //$("#" + gauge +" .metric").css("color", "#a12");
            $("#" + gauge +" .metric").animate({"color": "#33D010"}, 500);
        }else if(opts.newVal < 70)
        {
            rGopts.strokeColor = "#81810B";
            rGopts.colorStart = "#D0D010";
            $("#" + gauge +" .metric").animate({"color": "#D0D010"}, 500);
        }else{
            rGopts.strokeColor = "#740A0A";
            rGopts.colorStart = "#E81313";
            $("#" + gauge +" .metric").animate({"color": "#E81313"}, 500);
        }

        cf_rGs[gauge].setOptions(rGopts);
		cf_rGs[gauge].set(parseInt(opts.newVal));
	}
}