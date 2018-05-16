var express = require("express");
var bodyParser = require("body-parser");
var mongodb = require("mongodb");
var mongoose = require('mongoose');
var bcrypt = require('bcrypt');
var ObjectID = mongodb.ObjectID;
var User = require('../db/user');

var app = express();
app.use(bodyParser.json());

/* See docs/api.md for detailed method descriptions and return values */

mongoose.connect(process.env.MONGODB_URI || 'mongodb://heroku_7ktv9vft:811ufk5iq4as8u5peplshtm31d@ds119489.mlab.com:19489/heroku_7ktv9vft');
if (process.env.NODE_ENV === 'production') {
  app.use(express.static('client/build'));
}

var db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', function() {
  console.log("Database connection ready");

  // Save database object from the callback for reuse.
  // Initialize the app.
  var server = app.listen(process.env.PORT || 8080, function () {
    var port = server.address().port;
    console.log("App now running on port", port);
  });
});

// API Endpoints

// Generic error handler used by all endpoints.
function handleError(res, reason, message, code) {
  console.log("ERROR: " + reason);
  res.status(code || 500).send({"error": message});
}

/*  "/api/users"
 *    GET: shows all users, or a single user
 *    DELETE: deletes a user given by an id
 *    PUT: Update a given user's password, name, or email
 */
app.get("/api/users", function(req, res, next) {
  User.find({}, {'name': 1, 'email': 1} , function(err, users){
    if(err){
      handleError(res, err.message, "Failed to get list of users.", 500);
    }
    else{
      res.status(200).send(users);
    }
  });
});

// Return simple info about a user
app.get("/api/users/:id", function(req, res) {
  User.find({'_id': req.params.id}, {'name': 1, 'email': 1} , function(err, users){
    if(err){
      handleError(res, err.message, "Failed to get list of users.", 500);
    }
    else{
      res.status(200).send(users);
    }
  });
});

// Update various parts of a user id
app.put("/api/users/:id/:variable", function(req, res, next){
  if(req.body.updater){
    if (req.params.variable === 'password'){
      // Verify that old and new passwords match
      User.findOne({ _id: req.params.id }, function(err, user) {
        if (err) {
          handleError(res, err, 'Database error', 500);
          return 500;
         }
        if (user === null) {
          res.status(401).send('Incorrect username / password.');
          return 401;
        }
        else{
          user.validPassword(req.body.old, function(err, match){
            if (err){
              console.log(err)
            }
            if (match){ // matching password
              // hash new password and change
              bcrypt.hash(req.body.updater, 10, function (err, hash){
                if (err) {
                  handleError(res, err.message, "Failed to hash new pw", 500);
                }
                else{
                  User.findByIdAndUpdate(req.params.id, {'password': hash} , function(error, user){
                    if(error){
                      handleError(res, error.message, "Failed to update password", 500);
                    }
                    else{
                      res.status(200).send("Password updated")
                    }
                  });
                }
              });
            }
            else {
              res.status(401).send('Incorrect current password.');
              return 401;
            }
          });

        }
      });


    }

    else if (req.params.variable === 'email' || req.params.variable === 'name'){
      var requestedChange = req.params.variable;
      var project = {};
      project[requestedChange] = req.body.updater;
      User.findByIdAndUpdate(req.params.id, project, function(error, user){
        if(error){
          handleError(res, error.message, "Failed to update", 500);
        }
        else if(user){
          res.status(200).send("updated");
        }
        else{
          res.status(400).send("User not found");
        }
      });
    }
    else {
      handleError(res, "Update variable not recognized", "Use one of: password, email, name", 400);
    }
  }
  else{
    handleError(res, "Provide item to update", "Use one of: password, email, name", 400);
  }
});

// Delete a user
app.delete("/api/users/:id", function(req, res) {
    User.findByIdAndRemove(req.params.id, function(error, users){
      if (error){
        handleError(res, err.message, "Failed to delete user", 500);
      }
      else {
        res.status(200).send("User successfully deleted");
      }
    })
});

/*  "/api/register"
 *    POST: creates a new user
 */
app.post("/api/register", function(req, res) {
  if (req.body.email &&
    req.body.name &&
    req.body.password){
      var newUser = {
        email: req.body.email,
        name: req.body.name,
        password: req.body.password
      }

      User.create(newUser, function (error, user) {
        if (error) {
          console.log(error);
          res.status(500).send("User already exists");
        }
        else {
          res.status(200).send(user._id);
        }
      });
    }
});

/*  "/api/login"
 *    POST: Login an existing user
 */
 // Returns user id which can be used for other requests
 app.post('/api/login', function(req, res, next) {
   User.findOne({ email: req.body.email }, function(err, user) {
     if (err) {
       handleError(res, err, 'Database error', 500);
       return 500;
      }
     if (user === null) {
       res.status(401).send('Incorrect username / password.');
       return 401;
     }
     else{
       user.validPassword(req.body.password, function(err, match){
         if (err){
           console.log(err)
         }
         if (match){ // matching password
           res.status(200).send(user._id);
           return 200;
         }
         else {
           res.status(401).send('Incorrect username / password.');
           return 401;
         }
       });

     }
   });
 });

/* "/api/gifs/:id"
 *    GET: show saved gifs given user id
 *    PUT: save a given gif to a user's account
 *    DELETE: Remove a given gif from the collection of a given user
 */
app.get("/api/gifs/:id", function(req, res) {
  User.find({'_id' : { $eq: req.params.id}}, {'gifs': 1} , function(err, users){
    if(err){
      handleError(res, err.message, "Failed to get list of gifs", 500);
    }
    else{
      res.status(200).send(users);
    }
  });
});

// Save a new gif
app.put("/api/gifs/:id", function(req, res) {
  if (req.body.gif){
    // Push new gif to gifs array
    User.findByIdAndUpdate(req.params.id, {"$addToSet": {gifs: req.body.gif}}, function(err, users){
      if(err){
        handleError(res, err.message, "Failed to update gif list", 500);
      }
      else{
        res.status(200).send("Gif added");
      }
    });
  }
  else{
    handleError(res, "Failed to update gif list", "Gif is required", 400);
  }
  });

// Remove a saved gif
app.delete("/api/gifs/:id", function(req, res) {
  if (req.body.gif){
    // Pull gif from gifs array
    User.findByIdAndUpdate(req.params.id, {"$pull": {gifs: req.body.gif}}, function(err, users){
      if(err){
        handleError(res, err.message, "Failed to update gif list", 500);
      }
      else{
        res.status(200).send("Gif removed");
      }
    });
  }
  else{
    handleError(res, "Failed to update gif list", "Gif is required", 400);
  }
});
