function begin_authorization() {
    document.getElementById("sign_in").style.display = "none";
    document.getElementById("content").style.display = "none";
    document.getElementById("auth").style.display = "flex";
}

function authorized_user() {
    document.getElementById("sign_out").style.display = "inline-block";
    document.getElementById("sign_in").remove();
    document.getElementById("auth").remove();
}

function unauthorized_user() {
    document.getElementById("sign_out").style.display = "inline-block";
    document.getElementById("settings_page").remove();
    document.getElementById("sign_out").remove();
    document.getElementById("user_name").remove();
    document.getElementById("user_avatar").remove();
}

function not_admin() {
    document.getElementById("settings_page").remove();
}

function email_is_unknown() {
    document.getElementById("user_avatar").remove();
}

$(document).on("click", "#sign_in *", function() {
    begin_authorization();
    return false;
});

$(document).ready(function() {
    if ($("#input_user_name")) {
        $("#input_user_name").val(localStorage.getItem("user_name"));
    }
});

