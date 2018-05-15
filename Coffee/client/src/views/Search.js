import React, { Component } from "react";
import { NavLink} from "react-router-dom";
import cafe1 from '../images/cafe1.png';
import '../style/Search.css';

// Search for gifs
class Search extends Component {
    constructor(props) {
    super(props);

    this.state = {
      gifs: [],
      query: ""
    };
    this.grabImages = this.grabImages.bind(this);
    this.doSearch = this.doSearch.bind(this);
    this.handleChange = this.handleChange.bind(this);
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
       <div className="searchBar">
         <h1> Search</h1>
        <form onSubmit={this.doSearch}>
          <span>
            <input type="text" className="txtbox" placeholder="Search for gifs" value={this.state.query} onChange={this.handleChange} />
            <input type="image" name="submit" src={cafe1} className="cafesize" value="" alt="Submit" />
          </span>
        </form>
        <div className="gifbox" ref="gifContainer">
          {renderedGifs}
        </div>



      </div>
    );
  }

  // Turn returned data into list of images
  grabImages(data){
      var len = data.length;
      var images = [];
      for (var i = 0; i < len; i++){
        images.push(data[i].images.fixed_height.url);
      }

      this.setState({gifs: images})
   }

  // Updates the query in the pages state on every keystroke
  handleChange(event) {
    this.setState({query: event.target.value});
  }

  // Search for gifs using giphy api. Based on code from class
  doSearch(e) {
      e.preventDefault(); // prevents a refresh when function is called
      fetch("https://api.giphy.com/v1/gifs/search?api_key=vRvUFu9f8SrzzJWqp9b7aiIKTqKExxA2&q="+this.state.query)
      .then(response => {  ////
          // console.log(response.status, response.statusCode)
          if (response.ok) {
              return response.json()
          } else {
              throw new Error("No Gifs available for this keyword");
          }
      })
      .then(json => {
          // console.log("Response ",json)
          this.grabImages(json.data)

      })
      .catch(error => console.log(error))
      }
}

export default Search;
