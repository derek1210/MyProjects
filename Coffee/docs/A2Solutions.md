# Coffee Gif Browser

## Getting Started

We made our app with React. In order to view our SPA, you should do the following:

```
$ git clone https://github.com/csc309-18s/assignment-2-coffee.git
$ cd assignment-2-coffee
$ npm install
$ npm start
```
Your browser should now open the URL `localhost:3000`; where our SPA lives. A window may pop up asking you to allow incoming connections.
Please click 'yes'.

## Source Code

Source code for all views is located in [src/views](/src/views).
Source code for the header and footer is located in [src/App.js](/src/App.js). The `content` div in `App.js` is
where views are swapped into based on user input. CSS style for all views is located in [src/style](/src/style)

## Using Coffee

Our SPA has eight views: `Trending`, `Search`, `Translate`, `Share`, `Profile`, `Log In`, `Register` and `Reset`. Most of these can be accessed through
clicking the link to them in the navigation bar at the top of the app. `Share` is accessed through clicking any gif in the application. `Register` and `Reset` are only available from the `Log In` view.

### Trending
`Trending` is the default view of our application, where a user will start from. It displays the top 25 trending gifs from Giphy. If a user clicks on a gif,
they are directed to the `Share` view.


### Search & Translate

`Search` and `Translate` are very similar. Both can be accessed from the top nav-bar. They allow a user to search for gifs from Giphy (`Search`), or allow a user to translate
their phrases into gif form (`Translate`). `Search` returns 25 gifs to the user, while `Translate` returns a single gif. If a user clicks on a gif,
they are directed to the `Share` view.

### Share

Users can arrive on the `Share` view by clicking a gif in any other view.
The gif they clicked is displayed, along with links
to share the gif on Facebook or Twitter. Eventually, the `Share` view will also allow a user to save a gif to their profile.

### Profile

The `Profile` view will eventually display the saved gifs of a logged in user.
It can be accessed from the nav-bar Since we haven't implemented this feature, as it requires
some work on the back end, `Profile` only displays five trending gifs at the moment.

### Log-in, Register and Reset

The `Log In` view is also a work in progress. Right now it has no functionality, as this will all be built in phase 3. Eventually this
is where a user will log in to our application. It can be accessed by clicking the link in the top right of the screen. The `Register` and `Reset` views are accessible from the
`Log In` view, they also currently have no functionality. Eventually, these views will
allow a user to register for our application or reset their password.
