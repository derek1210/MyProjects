import React, { Component } from "react";

class Profile extends Component {
    constructor(props) {
        super(props);
    
        this.state = {
          gifs: []
        };
        this.grabImages = this.grabImages.bind(this);
        this.randomGifs = this.randomGifs.bind(this);
        this.randomGifs();
    }

    render () {
        var renderedGifs = this.state.gifs.map(function (gif, i){
            return (<img src={gif} key={i} alt="Gif failed to load" class='gif'/>)
        }); 
        return (
            <div> 
                <h1 style={{color : 'white'}}>My Gifs</h1>
                <div class="gifbox" ref="gifContainer">
                    {renderedGifs}
                </div>  
            </div>
        );
    }

    grabImages(data){
        var len = data.length;
        var images = [];
        for (var i = 0; i < len; i++){
          images.push(data[i].images.fixed_height.url);
        }
    
        this.setState({gifs: images})
    
    }

    randomGifs() {
        fetch("http://api.giphy.com/v1/gifs/trending?api_key=vRvUFu9f8SrzzJWqp9b7aiIKTqKExxA2&limit=5")
        .then(response => {
            console.log(response.status, response.statusCode)
                if (response.ok) {
                    return response.json()
                } else {
                    throw new Error("No Gifs");
                }
            })
            .then(json => {
                console.log("Response ",json)
                this.grabImages(json.data)

        })
        .catch(error => console.log(error))
    }
}

export default Profile;
