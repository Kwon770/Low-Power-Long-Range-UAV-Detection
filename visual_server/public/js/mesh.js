// Keep track of our socket connection
let socket;

let VerletPhysics2D = toxi.physics2d.VerletPhysics2D,
    VerletParticle2D = toxi.physics2d.VerletParticle2D,
    VerletSpring2D = toxi.physics2d.VerletSpring2D,
    VerletMinDistanceSpring2D = toxi.physics2d.VerletMinDistanceSpring2D,
    Vec2D = toxi.geom.Vec2D,
    Rect = toxi.geom.Rect;

let options = {
    nodeRadius: 40,
    ageThreshold: 30000,
    springStrength: 0.001,
    minDistanceSpringStrength: 0.05,
    strengthScale: 5.0
};

let mesh,
    nodes,
    physics,
    nodeColors;

let dataCounter = 0;
let bottomPadding = 50;
let selectedNode;
let showIndirectRoutes = true;

// utility to provide an iterator function with everly element
// and every element after that element
function forEachNested(arr, fn){
    for(let i=0; i<arr.length; i++){
        for(let j=i+1; j<arr.length; j++){
            let result = fn(arr[i], arr[j], i, j, arr);
            if(result === false){
                return;
            }
        }
    }
}


function setup() {
  createCanvas(window.innerWidth, window.innerHeight - bottomPadding);
  textSize(18);
  nodeColors = [color('#ff0000'), color('#ffcc00'), color('#0000ff'), color('#00bb00')];

  physics = new VerletPhysics2D();
  physics.setWorldBounds(new Rect(10, 10, width-20, height-20));
  physics.clear();
  mesh = new Mesh();

  // Start a socket connection to the server
  socket = io.connect(getURL());
  // We make a named event called 'mouse' and write an
  // anonymous callback function
  socket.on('mesh-data',
    // When we receive data
    function(data) {
      let nodeInfo = JSON.parse(data);
      let nodeKey = Object.keys(nodeInfo)[0];
      let nodeNumber = parseInt(nodeKey); // assume one property which is the node name
      let routes = nodeInfo[nodeKey];
      console.log("updating node " + nodeNumber + " with routes " + JSON.stringify(routes));
      mesh.updateNode(nodeNumber, routes);
    }
  );
}

function mousePressed() {
  Object.values(mesh.nodes).forEach(function (n) {
    let dx = mouseX - n.x;
    let dy = mouseY - n.y;
    if (sqrt((dx*dx) + (dy*dy)) < (options.nodeRadius/2)) {
      selectedNode = n;
    }
  });
  if (!selectedNode) {
    showIndirectRoutes = !showIndirectRoutes;
  }
}

function mouseReleased() {
  selectedNode = null;
}

function mouseDragged() {
  if (selectedNode) {
    selectedNode.x = mouseX;
    selectedNode.y = mouseY;
  }
}

function dashedLine(x1, y1, x2, y2, c1, c2) {
  let dx = x2-x1;
  let dy = y2-y1;
  let d = sqrt((dx*dx) + (dy*dy));
  let steps = d / 40;
  let ix1, iy1, ix2, iy2;
  let mx, my;
  stroke(c1);
  line(x1, y1, x2, y2);
  for(let i=0; i<steps; i++) {
    ix1 = lerp(x1, x2, i/steps);
    iy1 = lerp(y1, y2, i/steps);
    ix2 = lerp(x1, x2, (i+1)/steps);
    iy2 = lerp(y1, y2, (i+1)/steps);
    mx = (ix2+ix1)/2.0;
    my = (iy2+iy1)/2.0;
    if (i < (steps-1)) {
      stroke(c2);
      //line(mx, my, ix2, iy2);
      ellipse(ix1, iy1, 2, 2);
    }
  }

}


// For testing
/*
function mouseClicked() {
  socket.emit('mouse-click', dataCounter++);
}
*/

function draw(){

    // update the physics world. This moves the particles around.
    // after the movement, we can draw the particles and the connections.
    physics.update();

    background(255);

    // display all points
    mesh.display();

}


function Mesh(){
    this.nodes = {}; // map of nodes
}

// n is number, routes is object with route tables
Mesh.prototype.updateNode = function(n, routes) {
    let nodeKey = n.toString();
    let node = this.nodes[nodeKey];

    if (!node) {
        let pos = new Vec2D((width/2) + random(-50, 50), (height/2) + random(-50, 50));
        node = new Node(n, routes, pos);
        this.nodes[nodeKey] = node;
        console.log('created node ' + nodeKey);
    } else {
      node.routes = routes;
    }
    node.lastUpdate = new Date();

    // create a spring between each pair of nodes to keep them
    // at a minimum distance
    forEachNested(Object.values(this.nodes), function(n1, n2) {
      physics.addSpring(
        new VerletMinDistanceSpring2D(
          n1,
          n2,
          100,
          0.01
        )
      );
    });

    for (let r=1;r<=routes.length;r++) {
      if (r == n) continue; // self
      let route = routes[r-1];
      let nextNodeKey = route.n;
      let nextNode = parseInt(route.n);
      if (nextNode == 0) {
        console.log('route to node ' + r + ' is unknown');
      } else {
        if (r == nextNode) {
          // direct route
          let strength = abs(route.r);
          console.log('route to node ' + r + ' is direct with strength ' + strength);
          let node2 = this.nodes[nextNodeKey];
          if (node2) {
            // see if there is already a spring between the nodes. If so, remove it.
            let spring = physics.getSpring(node, node2);
            if (spring) {
              //console.log('--- removed spring between ' + n + ' and ' + nextNode);
              physics.removeSpring(spring);
            }
            spring = physics.getSpring(node2, node);
            if (spring) {
              //console.log('--- removed spring between ' + nextNode + ' and ' + n);
              physics.removeSpring(spring);
            }
            let node2Routes = node2.routes;
            let reverseRoute = node2Routes[n-1];
            if (parseInt(reverseRoute.n) == n) {
              //console.log('found reverse route from ' + nextNode + ' to ' + n + ': ' + JSON.stringify(reverseRoute));
              let strength2 = abs(reverseRoute.r);
              strength = (strength + strength2) / 2.0;
              //console.log('using average strength = ' + strength);
            }
            physics.addSpring(
                new VerletSpring2D(
                    node,
                    node2,
                    strength * options.strengthScale,
                    options.springStrength
                )
            );
            //console.log('+++ added spring between ' + n + ' and ' + nextNode + ' with strength ' + strength);
          }
        } else {
          console.log('route to node ' + r + ' is via node ' + nextNode);
        }
      }
    }

    console.log('');
}

Mesh.prototype.display = function() {
    let nodeNums = Object.keys(this.nodes);
    for(let i=0;i<nodeNums.length;i++) {
      let n = this.nodes[nodeNums[i]];
      let now = new Date();
      if (now.getTime() - n.lastUpdate.getTime() > options.ageThreshold) {
        continue;
      }
      fill(nodeColors[n.nodeNum - 1]);
      for(let j=0;j<n.routes.length;j++) {
        let r = n.routes[j];
        if (r.n != '255') { // if not self
          let n2, via;
          let direct = false;
          if (r.n == (j+1).toString()) {
            // direct route
            direct = true;
            n2 = this.nodes[r.n];
          } else {
            // indirect route
            n2 = this.nodes[(j+1).toString()];
            via = this.nodes[r.n];
          }
          if (n2) {
            let m = (n.y-n2.y)/(n.x-n2.x);
            let mp = -(n.x-n2.x)/(n.y-n2.y); // slope of perpendicular
            let q = sqrt(1.0/(1.0 + (mp*mp)));
            let tx, ty;
            let tOffset = 15;
            let lineWeight = 3;
            strokeWeight(lineWeight);
            let lineOffset = lineWeight/2.0;
            let s = r.r.toString();
            if (n.x < n2.x) {
              if (direct && (r.r != 0)) {
                // direct route. Draw solid line
                stroke(nodeColors[n.nodeNum - 1]);
                line(n.x + (lineOffset*q), n.y + (lineOffset*mp*q), n2.x + (lineOffset*q), n2.y + (lineOffset*mp*q));
                tx = n.x + abs(n.x-n2.x)/2.0;
                if (n.y < n2.y) {
                  ty = n.y + abs(n.y-n2.y)/2.0;
                } else {
                  ty = n.y - abs(n.y-n2.y)/2.0;
                }
                tx = tx + (tOffset*q);
                ty = ty + (tOffset*mp*q);
                strokeWeight(1);
                text(s, tx, ty);
              } else {
                // indirect route, draw dashed line
                if (showIndirectRoutes && via) {
                  dashedLine(n.x + (lineOffset*q), n.y + (lineOffset*mp*q), n2.x + (lineOffset*q), n2.y + (lineOffset*mp*q), nodeColors[n.nodeNum - 1], nodeColors[via.nodeNum - 1]);
                }
              }
            } else {
              if (direct && (r.r != 0)) {
                // direct route. Draw solid line
                stroke(nodeColors[n.nodeNum - 1]);
                line(n.x - (lineOffset*q), n.y - (lineOffset*mp*q), n2.x - (lineOffset*q), n2.y - (lineOffset*mp*q));
                tx = n.x - abs(n.x-n2.x)/2.0;
                if (n.y > n2.y) {
                  ty = n.y - abs(n.y-n2.y)/2.0;
                } else {
                  ty = n.y + abs(n.y-n2.y)/2.0;
                }
                tx = tx - (tOffset*q) - textWidth(s);
                ty = ty - (tOffset*mp*q);
                strokeWeight(1);
                text(s, tx, ty);
              } else {
                // indirect route, draw dashed line
                if (showIndirectRoutes && via) {
                  dashedLine(n.x - (lineOffset*q), n.y - (lineOffset*mp*q), n2.x - (lineOffset*q), n2.y - (lineOffset*mp*q), nodeColors[n.nodeNum - 1], nodeColors[via.nodeNum - 1]);
                }
              }
            }
          }
        }
      }
    }

    Object.values(this.nodes).forEach(function (n) {
      n.display();
    });

};



// Node inherits from `toxi.physic2d.VerletParticle2D`
// and adds a `display()` function for rendering with p5.js
function Node(nodeNum, routes, pos){
    // extend VerletParticle2D!
    this.nodeNum = nodeNum;
    this.routes = routes;
    VerletParticle2D.call(this, pos);
}

Node.prototype = Object.create(VerletParticle2D.prototype);

Node.prototype.display = function(){
    fill(nodeColors[this.nodeNum - 1]);
    stroke(0, 50);
    //noStroke();
    ellipse(this.x, this.y, options.nodeRadius, options.nodeRadius);
    fill('white');
    let s = "" + this.nodeNum;
    let tx = this.x - textWidth(s)/2.0;
    let ty = this.y + textAscent()/2.0 - textDescent()/2.0;
    text(this.nodeNum, tx, ty);
};
