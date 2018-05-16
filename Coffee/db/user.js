var mongoose = require('mongoose');
var bcrypt = require('bcrypt');

// Schema for our database. Based on code from class and mongo docs
var userSchema = new mongoose.Schema({
  email: {
    type: String,
    unique: true,
    required: true,
    trim: true
  },
  name: {
    type: String,
    required: true,
    trim: true
  },
  password: {
    type: String,
    required: true,
  },
  gifs: {
    type: Array
  }
});

// For login authentication
userSchema.methods.validPassword = function(pwd, callback) {
  bcrypt.compare(pwd, this.password, function(err, res) {
    if (err) {
      return callback(err);
    }
    callback(null, res);
  });
};

// Store password securely.
// From https://medium.com/of-all-things-tech-progress/starting-with-authentication-a-tutorial-with-node-js-and-mongodb-25d524ca0359
userSchema.pre('save', function (next) {
  var user = this;
  bcrypt.hash(user.password, 10, function (err, hash){
    if (err) {
      return next(err);
    }
    user.password = hash;
    next();
  });
});

module.exports = mongoose.model('User', userSchema);
