const iotRoute = require('./iot')
function route(app){

    app.use('/',iotRoute)

}
module.exports = route