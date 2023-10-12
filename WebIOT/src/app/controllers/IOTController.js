class IOTController{
    index(req,res){
        res.render('dashboard')
    }
}
module.exports = new IOTController