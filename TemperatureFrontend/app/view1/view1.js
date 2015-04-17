'use strict';

angular.module('myApp.view1', ['ngRoute', 'nvd3'])

.config(['$routeProvider', function($routeProvider) {
  $routeProvider.when('/current', {
    templateUrl: 'view1/view1.html',
    controller: 'View1Ctrl'
  });
}])

.controller('View1Ctrl', ['$scope', '$http', function($scope,$http) {


	$scope.currentTemprature = 0;

	var allDataPoints = [];

	setInterval(function(){updateTemperature()}, 5000);

	function updateTemperature(){
	
		$http.get('http://localhost:3000/temperature').success(function(data){
			console.log(data);
			var time = new Date();

			var datapoint = {
				x: time, y: data.latestTemperature
			}

			allDataPoints.push(datapoint);

			console.dir(time);
			$scope.currentTemprature = data.latestTemperature;
		});

	}

	  /* Chart options */
        $scope.options = {
            chart: {
                type: 'lineChart',
                height: 450,
                margin : {
                    top: 20,
                    right: 20,
                    bottom: 40,
                    left: 55
                },
                x: function(d){ return d.x; },
                y: function(d){ return d.y; },
                useInteractiveGuideline: true,
                dispatch: {
                    stateChange: function(e){ console.log("stateChange"); },
                    changeState: function(e){ console.log("changeState"); },
                    tooltipShow: function(e){ console.log("tooltipShow"); },
                    tooltipHide: function(e){ console.log("tooltipHide"); }
                },
                xAxis: {
                    axisLabel: 'Date and Time',
                    tickFormat: function(d) {
				          return d3.time.format('%H:%M:%S')(new Date(d))
				      }
                },
                yAxis: {
                    axisLabel: 'Temperature (°C)',
                    tickFormat: function(d){
                        return d3.format('.02f')(d);
                    },
                    axisLabelDistance: 30
                },
                callback: function(chart){
                    console.log("!!! lineChart callback !!!");
                }
            },
            title: {
                enable: true,
                text: 'Temperatures'
            }
        };

        $scope.data = convertData();

        /*Random Data Generator */
        function convertData() {
 

            //Line chart data should be sent as an array of series objects.
            return [
                {
                    values: allDataPoints,      //values - represents the array of {x,y} data points
                    key: 'Temperature', //key  - the name of the series.
                    color: '#ff7f0e'  //color - optional: choose your own line color.
                }
            ];
        };
 }]);