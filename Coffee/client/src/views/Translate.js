import React, { Component } from "react";
import { NavLink} from "react-router-dom";
import cafe1 from '../images/cafe1.png';
import '../style/Search.css';

// Similar to search: takes a word or phrase and translates to gif form
class Translate extends Component {
  constructor(props) {
    super(props);

    this.state = {
      gifs: ['https://media2.giphy.com/media/l44QBLHOuGfibb0xG/200.gif'],
      query: ""
    };
    this.grabImages = this.grabImages.bind(this);
    this.doTranslate = this.doTranslate.bind(this);
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
       <h1>Translate</h1>
      <form onSubmit={this.doTranslate}>
          <span>
            <input type="text" className="txtbox" placeholder="Translate your phrases into gifs"
             value={this.state.query} onChange={this.handleChange} />
            <input type="image" name="submit" src={cafe1} className="cafesize" value="" alt="Submit" />
          </span>
      </form>
      <div className="gifbox" ref="gifContainer">
        {renderedGifs}
      </div>
     </div>
   );
 }

 // Push the returned gif into a list to be rendered
  grabImages(data){
    var ims = []
    ims.push(data.images.fixed_height.url)
    this.setState({gifs: ims})
  }

  // On each keyboard action, update the translate query
  handleChange(event) {
    this.setState({query: event.target.value});
  }

  /* Use the giphy translation api to return user's query 'translated' into gif form
     Based on code from class */
  doTranslate(e) {
    e.preventDefault();
    fetch("https://api.giphy.com/v1/gifs/translate?api_key=vRvUFu9f8SrzzJWqp9b7aiIKTqKExxA2&s="+this.state.query)
    .then(response => {
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

export default Translate;
