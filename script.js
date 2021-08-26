var obj_article_list = document.getElementById('article-list');
var obj_article = document.getElementById('article');
var obj_article_title = document.getElementById('article-title');
var obj_article_content = document.getElementById('article-content');

var article_dir_map = new Map();
var request = new XMLHttpRequest();


function load_file(file_path, on_success){
	request.onreadystatechange = function () {
		if (request.readyState == 4 && (request.status == 0 || request.status == 200)) { 
			on_success(request.responseText); 
		}
	};
	request.open('GET', file_path);
	request.send();
}

function load_article_list(){
	load_file('article_list.txt', function(response){
		const article_list = response.split(/\r?\n/);
		let current_dir = '';
		for(const article_title of article_list){
			if(article_title == '') { 
				let obj_br = document.createElement('br');
				obj_article_list.appendChild(obj_br);
			} else if(article_title[0] == '#'){
				let obj_article_group = document.createElement('div');
				const article_group = article_title.split(' ');
				obj_article_group.className = 'article-group';
				obj_article_group.innerText = article_group[1];
				current_dir = article_group[2];
				obj_article_list.appendChild(obj_article_group);
			} else {
				let obj_article_link = document.createElement('a');
				obj_article_link.className = 'article-link';
				obj_article_link.href = '#'+ article_title;
				obj_article_link.innerText = article_title;
				article_dir_map[article_title] = current_dir;
				obj_article_list.appendChild(obj_article_link);
			}
		}
		load_article();
	});
}

function load_article(){
	article_title = decodeURI(location.hash.substring(1));
	if(article_title == ''){
		obj_article_list.style.display = 'block';
		obj_article.style.display = 'none';
		document.title = 'Chenqi\'s Blog';
	}else{
		obj_article_list.style.display = 'none';
		obj_article.style.display = 'block'; obj_article.scrollIntoView();
		document.title = article_title + ' - Chenqi\'s Blog';
		if(obj_article_title.innerText != article_title){
			obj_article_title.innerText = article_title;
			obj_article_content.innerHTML = '';
			load_file(article_dir_map[article_title] + '/' + article_title + '.md', function(response){
				obj_article_content.innerHTML = marked(response);
			});
		}
	}
}

window.onhashchange = load_article;
window.onload = load_article_list;