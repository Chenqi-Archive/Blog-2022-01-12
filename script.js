obj_article_list = document.getElementById('article-list');
obj_article = document.getElementById('article');
req = new XMLHttpRequest();

function load_file(file_path, on_success){
	req.onreadystatechange = function () {
		if (req.readyState == 4 && (req.status == 0 || req.status == 200)) { 
			on_success(req.responseText); 
		}
	};
	req.open('GET', file_path, true);
	req.send();
}

function load_article_list(){
	load_file('article_list.txt', function(response){
		var article_list = response.split(/\r?\n/);
		for(let i = 0; i < article_list.length; i++){
			if(article_list[i] == '') { 
				obj_article_list.appendChild(document.createElement('br'));
			} else {
				let obj_article_title = document.createElement('a');
				obj_article_title.className = 'article-title';
				obj_article_title.href = '#'+ article_list[i];
				obj_article_title.innerText = article_list[i];
				obj_article_list.appendChild(obj_article_title);
			}
		}
	});
}

function load_article(article_title) {
	if(article_title == ''){
		if(obj_article_list.innerHTML == ''){
			load_article_list();
		}
		obj_article_list.style.display = 'block';
		obj_article.style.display = 'none';
		document.title = 'Chenqi\'s Blog';
	}else{
		if(obj_article.dataset.title != article_title){
			obj_article.dataset.title = article_title;
			obj_article.innerHTML = '';
			load_file('articles/' + article_title + '.md', function(response){
				obj_article.innerHTML = marked(response);
			});
		}
		obj_article_list.style.display = 'none';
		obj_article.style.display = 'block';
		document.title = article_title + ' - Chenqi\'s Blog';
	}
}

function on_hash_change(){
	load_article(location.hash.substring(1));
}

function on_load(){
	on_hash_change();
}

window.onhashchange = on_hash_change;
window.onload = on_load;
load_article_list();