import React, { Component } from "react";
import { NavLink} from "react-router-dom";
import '../style/Search.css';

class Trending extends Component {
  constructor(props) {
    super(props);

    this.state = {
      gifs: []
    };
    this.grabImages = this.grabImages.bind(this);
    this.refreshTrends = this.refreshTrends.bind(this);
  }
  // Get updated trends on each refresh
  componentDidMount(){
    window.scrollTo(0, 0);
    this.refreshTrends();
  }

  render() {
    // Lambda function controls rendering of gifs
    var renderedGifs = this.state.gifs.map(function (gif, i){
        return (
          <NavLink to={'/share?gif='+ gif} key={i} className="gifLink">
            <img src={gif} key={i} alt="Gif failed to load" className='gif'/>
          </NavLink>)
    });
    return (
      <div>
        <div className="gifbox" ref="gifContainer">
          {renderedGifs}
        </div>
      </div>
    );
  }

  // Take away metadata and focus on the images
  grabImages(data){
    var len = data.length;
    var images = [];
    for (var i = 0; i < len; i++){
      images.push(data[i].images.fixed_height.url);
    }

    this.setState({gifs: images});

  }

  // Grab trending gifs from giphy. Based on code from class
  refreshTrends () {
    fetch("https://api.giphy.com/v1/gifs/trending?api_key=vRvUFu9f8SrzzJWqp9b7aiIKTqKExxA2")
    .then(response => {
      if (response.ok) {
        return response.json()
      } else {
        throw new Error("No Trends");
      }
    })
    .then(json => {
      // console.log("Response ",json)
      this.grabImages(json.data)

    })
    .catch(error => console.log(error))
  }


}

export default Trending;
