/*
Copyright 2018 Ola Benderius

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

const g_renderFreq = 4.0;
const g_maxDataAge = 10.0;
const g_maxFieldChartValues = 10;

var g_charts = new Map();
var g_chartConfigs = new Map();
var g_data = new Map();
var g_pause = false;

$(document).ready(function(){
  
  $('body').on('click', 'tr.dataHeader', function() {
    const idFieldsRow = $(this).attr('id') + '_fields';
    $('#' + idFieldsRow).toggleClass('hidden');
  });
  
  $('body').on('click', 'button#pause', function() {
    g_pause = !g_pause;
    if (g_pause) {
      $('button#pause').html('Continue');
    } else {
      $('button#pause').html('Pause');
    }
  });
  
  setupViewer();
});

function setupViewer() {
  var lc = libcluon();

  if ("WebSocket" in window) {
    var ws = new WebSocket("ws://" + window.location.host + "/");
    ws.binaryType = 'arraybuffer';

    ws.onopen = function() {
      onStreamOpen(lc);
    }

    ws.onmessage = function(evt) {
      onMessageReceived(lc, evt.data);
    };

    ws.onclose = function() {
      onStreamClosed();
    };

  } else {
    console.log("Error: websockets not supported by your browser.");
  }
}

function onStreamOpen(lc) {
  function getResourceFrom(url) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", url, false);
    xmlHttp.send(null);
    return xmlHttp.responseText;
  }

  var odvd = getResourceFrom("opendlv-standard-message-set-v0.9.1.odvd");

  console.log("Connected to stream.");
  console.log("Loaded " + lc.setMessageSpecification(odvd) + " messages from specification.");
  
  setInterval(onInterval, Math.round(1000 / g_renderFreq));
}

function onStreamClosed() {
  console.log("Disconnected from stream.");
}

function onMessageReceived(lc, msg) {

  if (g_pause) {
    return;
  }

  var data_str = lc.decodeEnvelopeToJSON(msg);

  if (data_str.length == 2) {
    return;
  }

  d = JSON.parse(data_str);


  // Translate to nice JSON ..
  var payloadFields = new Array();

  const payloadName = Object.keys(d)[5];
  
  for (const fieldName in d[payloadName]) {
    const fieldValue = d[payloadName][fieldName];
    const field = {
      name : fieldName,
      value : fieldValue,
      type : (typeof fieldValue)
    };
    payloadFields.push(field);
  }

  const data = {
    dataType : d.dataType,
    payload : {
      name : payloadName,
      fields : payloadFields
    },
    received : d.received,
    sampleTimeStamp : d.sampleTimeStamp,
    senderStamp : d.senderStamp,
    sent : d.sent
  };
  // .. done.

  const sourceKey = data.dataType + '_' + data.senderStamp;
  const dataSourceIsKnown = g_data.has(sourceKey);

  if (!dataSourceIsKnown) {
    addTableData(sourceKey, data);
    addFieldCharts(sourceKey, data);
    
    g_data.set(sourceKey, new Array());
  }
  
  storeData(sourceKey, data);
}

function toTime(t) {
  const milliseconds = t.seconds * 1000 + t.microseconds / 1000;
  return moment(milliseconds).format('YYYY-MM-DD hh:mm:ss');
}

function cutLongField(type, value) {
  if (type == 'string' && value.length > 20) {
    value = value.substr(0, 20) + ' <span class="dots">[...]</span>';
  }
  return value;
}

function addTableData(sourceKey, data) {
  
  if($('tr#' + sourceKey).length == 0) {

    const name = data.payload.name;
    const type = data.dataType;
    const sender = data.senderStamp;
    const timestamp = toTime(data.sampleTimeStamp);

    const headerHtml = '<tr id="' + sourceKey + '" class="dataHeader"><td>' 
      + type + '</td><td>' + sender + '</td><td>' + name + '</td><td id="' 
      + sourceKey + '_frequency">N/A</td><td id="' 
      + sourceKey + '_timestamp">' + timestamp + '</td></tr>';

    const fieldCount = data.payload.fields.length;

    var fieldsHtml = '<tr id="' + sourceKey + '_fields" class="hidden">'
      + '<td colspan="6"><table class="dataFields">';
    for (var i = 0; i < fieldCount; i++) {
    
      const field = data.payload.fields[i];
      const fieldName = field.name;   
      const fieldValue = cutLongField(field.type, field.value);
      
      fieldsHtml += '<tr><td class="field-name">' + fieldName + '</td>' 
        + '<td class="field-plot"><canvas id="' + sourceKey + '_field' 
        + i + '_canvas"></canvas></td><td id="' + sourceKey + '_field' 
        + i + '_value" class="field-value">' + fieldValue + '</td></tr>';
    }
    
    fieldsHtml += '</td></tr></table>';

    $('#dataView > tbody:last-child').append(headerHtml);
    $('#dataView > tbody:last-child').append(fieldsHtml); 
  }
}

function addFieldCharts(sourceKey, data) {

  const fieldCount = data.payload.fields.length;

  for (var i = 0; i < fieldCount; i++) {

    const field = data.payload.fields[i];
    const fieldType = field.type;
    
    if (fieldType == 'number') {

      const fieldKey = sourceKey + '_' + i;

      const fieldName = field.name;
      const fieldValue = field.value;
      
      const config = {
        type: 'line',
        data: {
          labels: [0.0],
          datasets: [{
            label: fieldName,
            data: [fieldValue],
            backgroundColor: 'rgb(255, 99, 132)',
            borderColor: 'rgb(255, 99, 132)',
            fill: false
          }]
        },
        options: {
          responsive: true,
          title: {
            display: false,
          },
          tooltips: {
            mode: 'index',
            intersect: false,
          },
          hover: {
            mode: 'nearest',
            intersect: true
          },
          legend: {
            display: false
          },
          animation: {
            duration: 0
          },
          scales: {
            xAxes: [{
              display: true,
              scaleLabel: {
                display: true,
                labelString: 'time'
              }
            }],
            yAxes: [{
              display: true,
              scaleLabel: {
                display: true,
                labelString: fieldName
              }
            }]
          }
        }
      };
      
      var ctx = document.getElementById(sourceKey + '_field' + i + '_canvas').getContext('2d');
      var chart = new Chart(ctx, config);
      
      g_charts[fieldKey] = chart;
      g_chartConfigs[fieldKey] = config;
    }
  }
}

function storeData(sourceKey, data) {
  
  const newDataTime = data.sent.seconds;
  const dataList = g_data.get(sourceKey);

  var dataPointsToShift = 0;
  for (var i = 0; i < dataList.length; i++) {
    const prevData = dataList[i];
    const dataTime = prevData.sent.seconds;
    const age = newDataTime - dataTime;
    
    if (age > g_maxDataAge) {
      dataPointsToShift++;
    } else {
      break;
    }
  }

  for (var i = 0; i < dataPointsToShift; i++) {
    g_data.get(sourceKey).shift();
  }
  g_data.get(sourceKey).push(data);
}

function onInterval() {
  if (g_pause) {
    return;
  }

  g_data.forEach(function(dataList, sourceKey, map) {
    const newestData = dataList[dataList.length - 1];
    updateTableData(sourceKey, newestData);
    updateFieldCharts(sourceKey, dataList);
  });
}

function updateTableData(sourceKey, data) {

  const dataList = g_data.get(sourceKey);
  if (dataList.length > 10) {
    const firstTimestamp = dataList[0].sent;
    const firstTime = firstTimestamp.seconds * 1000 + firstTimestamp.microseconds / 1000;
    const lastTimestamp = dataList[dataList.length - 1].sent;
    const lastTime = lastTimestamp.seconds * 1000 + lastTimestamp.microseconds / 1000;
    const frequency = Math.round(1000 * dataList.length / (lastTime - firstTime));
    $('td#' + sourceKey + '_frequency').html(frequency);
  } else {
    $('td#' + sourceKey + '_frequency').html('N/A');
  }

  const timestamp = toTime(data.sampleTimeStamp);
  $('td#' + sourceKey + '_timestamp').html(timestamp);

  const fieldCount = data.payload.fields.length;
  for (var i = 0; i < fieldCount; i++) {
    const field = data.payload.fields[i];
    const fieldValue = cutLongField(field.type, field.value);
    $('td#' + sourceKey + '_field' + i + '_value').html(fieldValue);
  }
}

function updateFieldCharts(sourceKey, dataList) {

  const latestData = dataList[dataList.length - 1];
  const latestTimestamp = latestData.sent;
  const latestTime = latestTimestamp.seconds * 1000 + latestTimestamp.microseconds / 1000;
  
  const fieldCount = latestData.payload.fields.length;
  
  // Clear charts.
  for (var i = 0; i < fieldCount; i++) {
    const field = latestData.payload.fields[i];
    const fieldType = field.type;
    if (fieldType == 'number') {
      const fieldKey = sourceKey + '_' + i;
      const config = g_chartConfigs[fieldKey];
      config.data.labels = new Array();
      config.data.datasets[0].data = new Array();
    }
  }
  
  // Add new data.
  const dataCount = dataList.length;
  const decimation = Math.floor(dataCount / (g_maxFieldChartValues - 1));
  for (var j = 0, k = 0; j < dataCount; j++, k++) {
    if (j != dataCount - 1) {
      if (k == decimation) {
        k = 0;
      }
      if (k != 0) {
        continue;
      }
    }
    
    const data = dataList[j];
    const timestamp = data.sent;
    const time = timestamp.seconds * 1000 + timestamp.microseconds / 1000; 
    
    const deltaTime = (time - latestTime) / 1000.0;
    
    for (var i = 0; i < fieldCount; i++) {
      const field = data.payload.fields[i];
      const fieldType = field.type;
    
      if (fieldType == 'number') {
    
        const fieldKey = sourceKey + '_' + i;
        const fieldValue = field.value;
      
        const config = g_chartConfigs[fieldKey];
   
        config.data.labels.push(deltaTime.toFixed(1));
        config.data.datasets[0].data.push(fieldValue);
      }
    }
  }
  
  // Update.
  for (var i = 0; i < fieldCount; i++) {
    const field = latestData.payload.fields[i];
    const fieldType = field.type;
    if (fieldType == 'number') {
      const fieldKey = sourceKey + '_' + i;
      const chart = g_charts[fieldKey];
      chart.update();
    }
  }
}
