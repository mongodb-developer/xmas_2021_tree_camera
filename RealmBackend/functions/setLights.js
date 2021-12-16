// This function is the endpoint's request handler.
exports = async function({ query, headers, body}, response) {


    try {
    const payload = JSON.parse(body.text());
    console.log(JSON.stringify(payload))
    state = payload.state;
    if(!state)  { response.StatusCode = 400; return "Missing state" }
    
    if(state.length != 50)  { response.StatusCode = 400; return "Must be 50 states"}
    
    newstate = [];
    for(x=0;x<state.length;x++) {
      r=state[x].r;
      g=state[x].g;
      b=state[x].b;
      if(r === undefined || g===undefined || b===undefined)  { response.StatusCode = 400; return `Missing r,g or b in ${JSON.stringify(state[x])}`;}
      r=Math.floor(r);g=Math.floor(g);b=Math.floor(b)
      if(r<0||r>255||g<0||g>255||b<0||b>255)  { response.StatusCode = 400; return "Value out of range"}
      newstate.push({r,g,b})
    }
    
  
      doc={device:"tree_1",state:newstate};
      const collection = context.services.get("mongodb-atlas").db("xmastree").collection("patterns")
      rval = await collection.insertOne(doc)
      response.StatusCode = 201; return rval;
    } catch(e) {
      console.error(e);
       response.StatusCode = 500; return `Internal error, Sorry. ${e}`;
    }
    return "Eh?"
};
