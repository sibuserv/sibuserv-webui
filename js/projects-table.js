// License: MIT (Expat)

function add_item_into_projects_table(i, item) {
    num = +localStorage.projects_number;
    localStorage.projects_number = num + 1;
    html =  "<li id=\"" + item.project_name + "\">\n" +
            "<a href=\"" + $("#projects_page").find("a").attr("href") + "/" +
                item.project_name + "\">" + item.project_name + "</a>\n" +
            "</li>\n";
    $("#projects_table").append(html);

    if (item.hasOwnProperty("last_project")) {
        if (item.last_project) {
            $("#show_more").remove();
        }
    }
}

function fill_in_projects_table(data) {
    array = $.parseJSON(data);
    $.each(array, add_item_into_projects_table);
}

function show_more_projects() {
    len = +localStorage.projects_per_page;
    pos = +localStorage.projects_number;
    data = "pos=" + pos + "&len=" + len;
    $.post("./?ajax=projects_list", data, fill_in_projects_table);
    return false;
}

function load_projects_table() {
    localStorage.projects_per_page = 7;
    localStorage.projects_number = 0;
    show_more_projects();
}

$(document).on("click", "#show_more", show_more_projects);
$(document).ready(load_projects_table);

