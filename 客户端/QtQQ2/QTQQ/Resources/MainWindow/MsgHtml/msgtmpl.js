var external0 = null;
var external_10001 = null;
var external_10003 = null;
var external_10004 = null;
var external_10005 = null;
var external_10006 = null;
var external_10009 = null;
var external_10010 = null;
var external_10012 = null;
var external_10013 = null;
var external_10014 = null;
var external_10015 = null;
var external_10016 = null;
var external_10017 = null;



String.prototype.format = function() {  
    if(arguments.length == 0) return this;  
    var obj = arguments[0];  
    var s = this;  
    for(var key in obj) {  
        s = s.replace(new RegExp("\\{\\{" + key + "\\}\\}", "g"), obj[key]);  
    }  
    return s;  
}  

new QWebChannel(qt.webChannelTransport,
	function(channel) {
		external0 = channel.objects.external0;
		external_10001 = channel.objects.external_10001;
external_10003 = channel.objects.external_10003;
external_10004 = channel.objects.external_10004;
external_10005 = channel.objects.external_10005;
external_10006 = channel.objects.external_10006;
external_10009 = channel.objects.external_10009;
external_10010 = channel.objects.external_10010;
external_10012 = channel.objects.external_10012;
external_10013 = channel.objects.external_10013;
external_10014 = channel.objects.external_10014;
external_10015 = channel.objects.external_10015;
external_10016 = channel.objects.external_10016;
external_10017 = channel.objects.external_10017;

	}
);

function appendHtml0(msg){
	$("#placeholder").append(external0.msgRHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};

function recvHtml_10001(msg){
	$("#placeholder").append(external_10001.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10003(msg){
	$("#placeholder").append(external_10003.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10004(msg){
	$("#placeholder").append(external_10004.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10005(msg){
	$("#placeholder").append(external_10005.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10006(msg){
	$("#placeholder").append(external_10006.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10009(msg){
	$("#placeholder").append(external_10009.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10010(msg){
	$("#placeholder").append(external_10010.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10012(msg){
	$("#placeholder").append(external_10012.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10013(msg){
	$("#placeholder").append(external_10013.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10014(msg){
	$("#placeholder").append(external_10014.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10015(msg){
	$("#placeholder").append(external_10015.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10016(msg){
	$("#placeholder").append(external_10016.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10017(msg){
	$("#placeholder").append(external_10017.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
