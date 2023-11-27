var values;
var perRef = [];
var periods = [];
var sheet;
var range;
var colNumb;
var perNot;
var stop = false;
var data = [];
var val2;
var val;
var elem2;
var elem;
var stat;
var stat2;
var perLen = 0;
var perInt = [];
var perIntCont = [];
var prevLine;
var nbLineBef;
var fileId = "1EoCDqXsL0tqAW6M7qVQT_N7L_NsBirMJ";
var values0;
var lenAgg;
var attributes;
var dataAgg;
var error = NaN;
var loner = NaN;

function onEdit(e) {
  prevLine = parseInt(PropertiesService.getScriptProperties().getProperty("prevLine"));
  sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  range = sheet.getDataRange();
  values = range.getValues();
  stat = values[0].length;
  colNumb = 0;
  for (var i = 5; i < stat; i++) {
    if (values[0][i] == "url") {
      colNumb=i+1;
      break;
    }
  }
  if(values.length<2 || colNumb == stat || stat<7) {
    if(stat<7) {
      if(colNumb == 0) {
        stat = 8;
        colNumb = 7;
      } else {
        stat = 7;
      }
    } else if(colNumb == stat) {
      stat++;
    }
    range = sheet.getRange(1,1,Math.max(3,values.length),stat);
    values = range.getValues();
  }
  if(colNumb == 0) {
    colNumb = Math.max(6, stat-1);
  }
  nbLineBef = values.length;
  if(isNaN(prevLine)) {
    prevLine = 1;
  }
  var range2 = sheet.getRange(1,1,Math.max(nbLineBef,prevLine)+1,colNumb+1);
  range2.clearDataValidations();
  var headLine = ["names", "following", "after", "before", ""];
  for(var i = 0; i<4; i++) {
    if(headLine[i]!=values[i]) {
      sheet.getRange(1, i+1).setValue(headLine[i]);
    }
  }
  if("periods"!=values[colNumb-2]) {
    sheet.getRange(1, colNumb-1).setValue("periods");
  }
  if("url"!=values[colNumb-1]) {
    sheet.getRange(1, colNumb).setValue("url");
  }

  //Each cell becomes a list
  var length = colNumb - stat;
  var fillSide = Array.from({ length: length });
  fillSide = fillSide.map(function (v, i) {return [];});
  values0 = JSON.parse(JSON.stringify(values));
  var toSortImg = false;
  for(var i = 1; i<nbLineBef; i++) {
    values[i] = values[i].concat(fillSide);
    for(var j = 0; j<colNumb+1; j++) {
      values[i][j] = values[i][j].toString().split(';').map(function (v, i) {return v.trim();}).filter(function(e) {
        return e !== ""});
    }
    if(values[0].length>=colNumb+2 && values[i][colNumb+2]) {
      var int = parseInt(values[i][colNumb+2]);
      if(!isNaN(int)) {
      }
    }
  }

  for(var i = nbLineBef-1; i>0; i--) {
    if(values[i].some((v,j)=>v.length!=0 && j<colNumb)) {
      break;
    }
    nbLineBef--;
  }
  values.splice(nbLineBef,values.length-nbLineBef);
  perNot = ["","start:","end:"];
  periods = [];
  perRef = [0];
  data = [0];
  perInt = [0];
  perIntCont = [0];
  var r;
  if(values[0].length>colNumb) {
    error = parseInt(values[1][colNumb+1]);
    loner = parseInt(values[2][colNumb+1]);
  }
  for(var i = 1; i<nbLineBef; i++) {
    perInt.push([]);
    perIntCont.push([[],[],[]]);
    perRef.push(-1);
    if(values[i][colNumb-1].length==0) {
      perRef[i] = -2;
    }
    data.push([true, [], [], -1]);
    for(var k = 0; k<values[i][0].length; k++) {
      isTip(i,0,k);
      //check if already before
      for(var r0 = 1; r0<=i; r0++) {
        if(r0==1) {
          r = i;
        } else {
          r = r0-1;
        }
        for(var f = 0; f<values[r][0].length; f++) {
          if(r==i && f>=k || !searching(r,0,f)) {continue;}
          if(i==r || stat==0 && (perRef[r]<0 || perRef[i]>-1 && (perRef[i]!=perRef[r] || periods[perRef[r]][0]!=i) ||
                perRef[i]<0 && periods[perRef[r]][0]>-1 || stat2==0) ||
              stat>0 && (perRef[i]>-1 && perRef[r]>-1 && perRef[i]!=perRef[r] && periods[perRef[r]][stat]>-1 ||
                perRef[i]<0 && perRef[r]>-1 && periods[perRef[r]][stat]>-1)) {
            elem = values[r][0][f];
            putSugg(i,k);
            return -1;
          }
          stop = true;
          break;
        }
        if(stop) {
          break;
        }
      }
      if(stat || stop) {
        if(!stop) {
          r = i;
          stat2 = stat;
        }
        if(found(r,i,0,k)==-1) {
          return -1;
        }
      }
      stop = false;
    }
  }
  var attNames = [];
  attributes = [];
  var acc = 0;
  for(var j = 4; j<colNumb-1; j++) {
    attNames.push([]);
    for(var i = 1; i<nbLineBef; i++) {
      for(var k = 0; k<values[i][j].length; k++) {
        isTip(i,j,k);
        if(stat) {
          values[i][j][k] = elem;
          sugg(i,j);
          return -1;
        }
        if(j<colNumb-2) {
          if(perRef[i]!=-1) {
            suggSet(i,j,[]);
            return -1;
          }
          for(var r = 0; r<attNames[j-4].length; r++) {
            if(attNames[j-4][r].toLowerCase()==val) {
              data[i][2].push(acc+r);
              attributes[acc+r]++;
              stop = true;
              break;
            }
          }
          if(!stop) {
            data[i][2].push(acc+attNames[j-4].length);
            attNames[j-4].push(val);
            attributes.push(1);
          }
        } else {
          for(var r = 1; r<nbLineBef; r++) {
            for(var f = 0; f<values[r][0].length; f++) {
              if(!searching(r,0,f)) {
                continue;
              }
              if(found(r,i,j,k)==-1) {
                return -1;
              }
              perInt[i].push(perRef[r]);
              stop = true;
              break;
            }
            if(stop) {
              break;
            }
          }
          if(!stop) {
            suggRef(i,j,k);
            return -1;
          }
        }
        stop = false;
      }
    }
    acc += attNames[j-4].length;
  }
  for(var i = 1; i<nbLineBef; i++) {
    for(var j = 1; j<4; j++) {
      for(var k = 0; k<values[i][j].length; k++) {
        isTip(i,j,k);
        for(var r = 1; r<values.length; r++) {
          for(var f = 0; f<values[r][0].length; f++) {
            if(searching(r,0,f)) {
              stop = true;
              break;
            }
          }
          if(stop) {
            break;
          }
        }
        if(!stop) {
          suggRef(i,j,k);
          return -1;
        }
        stop = false;
        if(stat>0) {
          if(found(r,i,j,k)==-1) {
            return;
          }
        }
        perIntCont[i][j-1].push([r,stat]);
        values[i][j][k] = perNot[stat] + elem2;
      }
    }
  }
  for(var i = 0; i<periods.length; i++) {
    for(var q = 1; q<3; q++) {
      if(periods[i][q] != -1) {
        continue;
      }
      periods[i][q] = values.length;
      values.push([]);
      for (let i = 0; i < colNumb; i++) {
        values[values.length-1].push([]);
      }
      for(var m = 0; m<periods[i][3].length; m++) {
        values[values.length-1][0].push(perNot[q]+periods[i][3][m]);
      }
      data.push([true,[],[],-1]);
    }
    for(var q = 1; q<3; q++) {
      values[periods[i][q]][0] = periods[i][3].map((e)=>perNot[q] + e).concat(
        values[periods[i][q]][0].filter((e)=>!e.startsWith(perNot[q])));
    }

    //fill start and end with the info on the declaration line
    var z = periods[i][1];
    var s = periods[i][2];
    data[s][1].push(z);
    var c = periods[i][0];
    if(c == -1) {
      c = values.length;
      values.push([]);
      for (let i = 0; i < colNumb; i++) {
        values[c].push([]);
      }
      for(var m = 0; m<periods[i][3].length; m++) {
        values[c][0].push(periods[i][3][m]);
      }
      data.push([true,[],[],z]);
      data[z][0] = false;
      continue;
    }
    data[z][0] = false;
    data[c][3] = z;
    /*for(var q = 1; q<3; q++) {
      for(var m = 0; m<values[c][q].length; m++) {
        values[z][q].push(values[c][q][m]);
      }
    }
    for(var m = 0; m<values[c][3].length; m++) {
      values[s][3].push(values[c][3][m]);
    }
    for(let m = 1; m<3; m++) {
      for(let j = 4; j<colNumb; j++) {
        for(let k = 0; k<values[c][j].length; k++) {
          if(!values[periods[i][m]][j].map((e)=>e.toLowerCase()).includes(values[c][j][k].toLowerCase())) {
            values[periods[i][m]][j].push(values[c][j][k]);
          }
        }
      }
    }*/
  }
  for(var i = 1; i<nbLineBef; i++) {
    for(var j = 0; j<perInt[i].length; j++) {
      if(perRef[i]>-1) {
        data[periods[perRef[i]][1]][1].push(periods[perInt[i][j]][1]);
        data[periods[perInt[i][j]][2]][1].push(periods[perRef[i]][2]);
      } else {
        data[i][1].push(periods[perInt[i][j]][1]);
        data[periods[perInt[i][j]][2]][1].push(i);
      }
    }
  }
  for(var i = 1; i<nbLineBef; i++) {
    for(var j = 0; j<3; j++) {
      for(var k = 0; k<perIntCont[i][j].length; k++) {
        var u = perIntCont[i][j][k];
        r = u[0];
        var e = r;
        var ie = i;
        if(perRef[i]>-1) {
          if(i==periods[perRef[i]][1]) {
            i = periods[perRef[i]][0];
          } else if(i==periods[perRef[i]][0]) {
            ie = periods[perRef[i]][2];
          }
        }
        if(perRef[r]>-1 && r==periods[perRef[r]][0]) {
          r = periods[perRef[r]];
          e = r[2];
          r = r[1];
        }
        if(u[1]==1) {
          e = r;
        } else if(u[1]==2) {
          r = e;
        }
        if(j==0) {
          if(!data[i][0]) {
            values[i][1].splice(1, values[i][1].length-1);
            sugg(i,1);
            return -1;
          }
          data[e][3] = i;
          data[i][0] = false;
        } else if(j==1) {
          data[i][1].push(e);
        } else {
          data[r][1].push(ie);
        }
      }
    }
  }

  // --search inconcistencies--

  var inLoop = [false];
  var follower = [0];
  for(var i = 1; i<data.length; i++) {
    if(data[i]===0) {
      inLoop.push(false);
    } else {
      inLoop.push(true);
    }
    follower.push(0);
  }
  //group lines by followings
  lenAgg = 0;
  dataAgg = [];
  for(var i = 1; i<data.length; i++) {
    if(!Array.isArray(data[i]) || !data[i][0]) {
      continue;
    }
    dataAgg.push({ lines: [], before: new Set(), after: new Set(), attr: [], mediaNb: 0});
    var j = i;
    var lenLines = 0;
    var mediaNb = 0;
    while(j!==-1) {
      dataAgg[lenAgg].lines.push(j);
      for(var k = 0; k<data[j][2].length; k++) {
        for(var h = 0; h<dataAgg[lenAgg].attr.length; h++) {
          if(dataAgg[lenAgg].attr[h][0] == data[j][2][k]) {
            dataAgg[lenAgg].attr[h][2] = mediaNb+1;
            dataAgg[lenAgg].attr[h][3]++;
            dataAgg[lenAgg].attr[h].push(mediaNb);
            stop = true;
            break;
          }
        }
        if(!stop) {
          h = dataAgg[lenAgg].attr.length;
          dataAgg[lenAgg].attr.push([data[j][2][k], mediaNb, mediaNb+1, 1, mediaNb]);
        }
        if(dataAgg[lenAgg].attr[h][3] == attributes[data[j][2][k]]) {
          var sum = 0;
          for (var m = 4; m < dataAgg[lenAgg].attr[h].length; m++) {
            sum += dataAgg[lenAgg].attr[h][m];
          }
          attributes[data[j][2][k]] = -Math.floor(sum/(dataAgg[lenAgg].attr[h].length-4));
        }
        stop = false;
      }
      if(!inLoop[j]) {
        var j0 = follower[j];
        var j2 = dataAgg[lenAgg].lines[lenLines];
        inconsist("Lines "+values[j0][0][0]+" ("+(j0+1)+")"+" and "+values[j2][0][0]+" ("+(j2+1)+")"+" both follow line "+j);
        return -1;
      }
      inLoop[j] = false;
      follower[j] = dataAgg[lenAgg].lines[lenLines];
      lenLines++;
      if(j<nbLineBef && perRef[j]==-1) {
        mediaNb++;
      }
      j = data[j][3];
    }
    dataAgg[lenAgg].mediaNb = mediaNb;
    lenAgg++;
  }
  //check if there is a loop that couldn't have been entered
  for(var i = 1; i<data.length; i++) {
    if(!Array.isArray(data[i]) || !inLoop[i]) {
      continue;
    }
    var j = i;
    var allLines = "";
    while(1) {
      var index = j+1;
      if(j>=nbLineBef) {
        index = "out";
      }
      allLines += values[j][0][0]+"("+index+") >> ";
      if(j==i) {
        break;
      }
      j = data[j][3];
    }
    inconsist(allLines);
    return -1;
  }
  //add the precedents to dataAgg
  var place = [];
  var visited = [];
  var steps = [];
  for(var i = 0; i<lenAgg; i++) {
    place.push([0,0]);
    visited.push(0);
    steps.push(0);
    for(var k = 0; k<dataAgg[i].lines.length; k++) {
      var f = dataAgg[i].lines[k];
      for(var h = 0; h<data[f][1].length; h++) {
        for(var m = 0; m<lenAgg; m++) {
          var ind = dataAgg[m].lines.indexOf(data[f][1][h]);
          if(ind!=-1) {
            if(i == m) {
              if(k<=ind) {
                var allLines = "";
                for(var v = k; v<ind+1; v++) {
                  var vi = dataAgg[i].lines[v];
                  var index = vi+1;
                  if(vi>=nbLineBef) {
                    index = "out";
                  }
                  allLines += " >> "+values[vi][0][0]+"("+index+")";
                }
                var vi = dataAgg[i].lines[k];
                var index = vi+1;
                if(vi>=nbLineBef) {
                  index = "out";
                }
                inconsist(allLines.slice(4)+" > "+values[vi][0][0]+"("+index+")");
                return -1;
              }
            } else {
              dataAgg[i].after.add(m);
              dataAgg[m].before.add(i);
            }
            break;
          }
        }
      }
    }
  }

  var stack = [];
  for (var objectId = 0; objectId<lenAgg; objectId++) {
    if (!visited[objectId] && dataAgg[objectId].after.size==0) {
      var path = [];
      stack.push(objectId);
      while (stack.length > 0) {
        var back = 0;
        var hasUnvisitedNeighbor = 0;
        const current = stack[stack.length - 1];
        if(path.length!=0 && path[path.length-1]==current) {
          back = 1;
        } else {
          path.push(current);
          visited[current] = 1;

          const neighbors = dataAgg[current].before;

          for (const neighbor of neighbors) {
            var taken = path.indexOf(neighbor);
            if (taken!=-1) {
              var allLines = "";
              path.push(neighbor);
              path.splice(0,taken);
              for(const i of path) {
                var sub = "";
                for(var j = 0; j<dataAgg[i].lines.length; j++) {
                  var x = dataAgg[i].lines[j];
                  var index = x+1;
                  if(x>=nbLineBef) {
                    index = "out";
                  }
                  sub += ","+values[x][0][0]+"("+index+")";
                }
                allLines = sub.slice(1) + " > "+allLines;
              }
              inconsist(allLines.slice(0,allLines.length-3));
              return -1;
            }
            if (steps[neighbor]) {
              var lvl = place[neighbor][0];
              dataAgg[lvl].before.delete(neighbor);
              dataAgg[neighbor].after.delete(lvl);
              if(place[neighbor][1]<stack.length && stack[place[neighbor][1]] == neighbor) {
                stack.splice(place[neighbor][1],1);
              }
            } else {
              steps[neighbor] = 1;
            }
            place[neighbor] = [current, stack.length];

            if (!visited[neighbor]) {
              stack.push(neighbor);
              hasUnvisitedNeighbor = 1;
            }
          }
        }
        if (back || !hasUnvisitedNeighbor) {
          path.splice(path.length-1,1);
          for (const neighbor of dataAgg[current].before) {
            steps[neighbor] = 0;
          }
          stack.pop();
        }
      }
    }
  }

  prevLine = 3;
  var colors = range2.getBackgrounds();
  var fonts = range2.getFontColorObjects();
  var color, font;
  for(var i = 0; i<colors.length; i++) {
    if(!i) {
      color = "000000";
      font = "ffffff";
    } else {
      font = "000000";
      if(perRef[i]>-1) {
        if(periods[perRef[i]][0] == i) {
          color = "ffa600";
        } else if(periods[perRef[i]][1] == i) {
          color = "93ff00";
        } else {
          color = "d97373";
        }
      } else if(perRef[i]==-2) {
        color = "73d9cb";
      } else {
        color = "ffffff";
      }
    }
    if(color!="ffffff" && prevLine<i) {prevLine = i;}
    if(colors[i].some(function(v,j) {return v!='#'+color && j<colNumb})) {
      sheet.getRange(i+1,1,1,colNumb).setBackground('#'+color);
    }
    if(fonts[i].some(function(v,j) {return v!='#'+font && j<colNumb})) {
      sheet.getRange(i+1,1,1,colNumb).setFontColor('#'+font);
    }
  }
  for(var i = 1; i<3; i++) {
    if(colors[i][colNumb]!="#000000") {
      sheet.getRange(i+1,colNumb+1).setBackground('#000000');
    }
    if(fonts[i][colNumb]!="#ffffff") {
      sheet.getRange(i+1,colNumb+1).setFontColor('#ffffff');
    }
  }
  if(colors[3][colNumb]=="#ffffff") {
    for(var i = 4; i<nbLineBef; i++) {
      if(colors[i][colNumb]!="#ffffff") {
        sheet.getRange(i+1,colNumb+1).setBackground('#ffffff');
      }
    }
  } else {
    sheet.getRange(4,colNumb+1,colors.length+10,1).setBackground("#ffffff");
    sheet.getRange(4,colNumb+1).setValue("");
  }
  if(fonts[3][colNumb]!="#000000") {
    sheet.getRange(4,colNumb+1).setFontColor('#000000');
  }
  if(colors[0][colNumb]!="#ffffff") {
    sheet.getRange(1,colNumb+1).setBackground("#ffffff");
  }
  if(fonts[0][colNumb]!="#000000") {
    sheet.getRange(1,colNumb+1).setFontColor('#000000');
  }
  var value = SpreadsheetApp.newRichTextValue()
    .setText("result")
    .setLinkUrl("https://drive.google.com/file/d/"+fileId+"/view")
    .build();
  sheet.getRange(1,colNumb+1).setRichTextValue(value);

  sheet.getRange(1,1,colors.length,colNumb).setWrap(false);
  exitAllways();
}

function suggRef(i,j,k) {
  var suggWords = [];
  var suggWords2 = [];
  for(var r = 1; r<nbLineBef; r++) {
    if(perRef[r]>-1 && periods[perRef[r]][0]!=r) {
      continue;
    }
    for(var f = 0; f<values[r][0].length; f++) {
      var val2 = values[r][0][f];
      if(val2.includes(val)) {
        suggWords.push(val2);
      } else if(val.includes(val2)) {
        suggWords2.push(val2);
      }
    }
  }
  suggWords.sort(function(a,b) {return a.indexOf(val)-b.indexOf(val)});
  suggWords2.sort(function(a,b) {return val.indexOf(a)-val.indexOf(b)});
  suggWords = suggWords.concat(suggWords2).map((v)=>values[i][j].slice(0,k).concat([perNot[stat]+v]).concat(values[i][j].slice(k+1)).join("; "));
  if(suggWords.length == 0) {
    values[i][j].splice(k,1);
    sugg(i,j);
  } else {
    suggSet(i,j,suggWords);
  }
}

function found(r,i,j,k) {
  if(perRef[r] == -1) {
    values[i][j].splice(k,1);
    sugg(i,j);
    return -1;
  }
  if(perRef[r] == -2) {
    perRef[r] = periods.length;
    periods.push([-1,-1,-1,values[r][0].filter(
      (e)=>e.startsWith(perNot[stat2])).map(
        (e)=>e.slice(perNot[stat2].length))]);
    periods[perRef[r]][stat2] = r;
  }
  if(j==0) {
    periods[perRef[r]][stat] = i;
    periods[perRef[r]][3] = periods[perRef[r]][3].concat(values[i][0].filter(
      (e)=>e.startsWith(perNot[stat])).map(
        (e)=>e.slice(perNot[stat].length)).filter(
          (e)=>!periods[perRef[r]][3].map((e)=>e.toLowerCase()).includes(e.toLowerCase())));
    perRef[i] = perRef[r];
  }
  if(i!=r) {
    values[i][j][k] = perNot[stat] + elem2;
  }
  return 1;
}

function isTip(i,j,k) {
  stat = 0;
  elem = values[i][j][k];
  val = elem.toLowerCase();
  for(var q = 1; q<3; q++) {
    if(val.startsWith(perNot[q])) {
      elem = values[i][j][k].slice(perNot[q].length).trim();
      val = elem.toLowerCase();
      if(val=="") {
        elem = values[i][j][k];
        putSugg(i,k);
        return -1;
      }
      stat = q;
      break;
    }
  }
}

function searching(r,d,f) {
  elem2 = values[r][d][f];
  val2 = elem2.toLowerCase();
  let stop2 = true;
  if(val2!=val) {
    stop2 = false;
    for(var q = 1; q<3; q++) {
      if(values[r][d][f].startsWith(perNot[q])) {
        elem2 = values[r][d][f].slice(perNot[q].length).trim();
        val2 = elem2.toLowerCase();
        if(val2==val) {
          stat2 = q;
          return true;
        }
      }
    }
  }
  stat2 = 0;
  return stop2;
}

function sugg(i,j) {
  suggSet(i,j,[values[i][j].join('; ')]);
}

function suggSet(i,j,sugg) {
  var rule = SpreadsheetApp.newDataValidation().requireValueInList(sugg).build();
  var cell = sheet.getRange(i+1,j+1);
  cell.setDataValidation(rule);
  var value = SpreadsheetApp.newRichTextValue()
    .setText("Error at "+cell.getA1Notation())
    .setLinkUrl(SpreadsheetApp.getActiveSpreadsheet().getUrl() + "#gid=" + sheet.getSheetId() + "&range=" + cell.getA1Notation())
    .build();
  sheet.getRange(4,colNumb+1).setRichTextValue(value);
  if(prevLine<i) {prevLine = i;}
  exit();
}

function inconsist(message) {
  sheet.getRange(4,colNumb+1).setValue(message);
  exit();
}

function exit() {
  sheet.getRange(4,colNumb+1,nbLineBef+10,1).setBackground("#ff0000");
  exitAllways();
}

function exitAllways() {
  PropertiesService.getScriptProperties().setProperty("prevLine", prevLine);
}

function putSugg(i,k) {
  while(!stop) {
    var theMatch = elem.match(/(.*? -)(\d)$/);
    if (theMatch) {
      elem = theMatch[1] + (parseInt(theMatch[2])+1);
    } else {
      elem += ' -2';
    }
    val = elem.toLowerCase();
    if(stat>0) {
      val = val.slice(perNot[stat].length);
    }
    stop = true;
    for(var m = 1; m<values.length; m++) {
      for(var p = 0; p<values[m][0].length; p++) {
        if((m!=i||p!=k) && searching(m,0,p)) {
          stop = false;
          break;
        }
      }
      if(!stop) {
        break;
      }
    }
  }
  values[i][0][k] = elem;
  sugg(i,0);
}

function correct() {
  for(var i = 1; i<nbLineBef; i++) {
    for(var j = 0; j<colNumb; j++) {
      var value = values[i][j].join('; ');
      if(values0[i][j]!=value) {
        values0[i][j] = value;
        sheet.getRange(i+1,j+1).setValue(value);
      }
    }
  }
}

function dataGenerator() {
  if(onEdit()==-1) {
    return -1;
  }
  correct();
  sheet.getRange(1,colNumb+2).setValue("loading...");
  var file = DriveApp.getFileById(fileId);
  if (file) {
    var spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
    var result = 'dataSpreadSheet\t'+lenAgg+'\t'+(data.length-1)+'\t'+attributes.length+'\t'+error+'\t'+loner+'\t'+spreadsheet.getName()+'_'+sheet.getName()+'\n'+
      attributes.join(',')+"\n"
      +dataAgg.map((row) =>
        [row.lines.join(','),row.after.size+','+row.mediaNb, 
          Array.from(row.before).join(','),
          row.attr.map((row)=>row.slice(0,3).join(',')).join('\t')
        ].join('\n')
      ).join('\n')+"\n"
      +values.map((row)=>row.map(function(cell) {
        if(Array.isArray(cell)) {
          return cell.join('; ');
        } else {
          return cell;
        }}).slice(0,colNumb).join('\t')).join('\n');
    //sheet.getRange(nbLineBef+10,colNumb + 1).setValue(result);
    file.setContent(result);
  }
  var currentDate = new Date();
  
  var hours = ('0' + currentDate.getHours()).slice(-2);
  var minutes = ('0' + currentDate.getMinutes()).slice(-2);
  var seconds = ('0' + currentDate.getSeconds()).slice(-2);
  
  var day = ('0' + currentDate.getDate()).slice(-2);
  var year = currentDate.getFullYear();

  var threeLetterMonth = currentDate.toLocaleString('en-US', { month: 'short' });
  
  var formattedDate = hours + ':' + minutes + ':' + seconds + ' ' + day + ' ' + threeLetterMonth + ' ' + year;
  sheet.getRange(1,colNumb+2).setValue("updated at "+formattedDate);
}

function renameSymbol() {
  if(onEdit() == -1) {
    return -1;
  }
  var nbRen = 0;
  for(var t = 0; t<values[1][colNumb].length; t++) {
    val = values[1][colNumb][t].toLowerCase();
    if(values[2][colNumb].length<=t) {
      elem = "";
    } else {
      elem = values[2][colNumb][t];
    }
    for(var i = 1; i<nbLineBef; i++) {
      for(var j = 0; j<colNumb; j++) {
        for(var k = 0; k<values[i][j].length; k++) {
          if(i==8 && j==6) {
            console.log("7");
          }
          if(searching(i,j,k)) {
            if(elem) {
              if(stat2>-1) {
                values[i][j][k] = perNot[stat2] + elem;
              } else {
                values[i][j][k] = elem;
              }
            } else {
              values[i][j].splice(k,1);
            }
            nbRen++;
          }
        }
      }
    }
  }
  correct();
  sheet.getRange(1,colNumb+2).setValue("renaming ("+nbRen+")");
}

function onOpen() {
  var menu = SpreadsheetApp.getUi().createMenu('Menu');
  menu.addItem('rename symbol', 'renameSymbol');
  menu.addItem('generate data', 'dataGenerator');
  menu.addToUi();
}


































