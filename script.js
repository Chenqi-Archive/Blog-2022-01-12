var obj_article_list = document.getElementById('article-list');
var obj_article = document.getElementById('article');
var obj_article_title = document.getElementById('article-title');
var obj_article_content = document.getElementById('article-content');

var request = new XMLHttpRequest();

function load_file(path) {
	return new Promise(resolve => {
		request.onload = () => resolve(request.responseText);
		request.open('GET', path); request.send();
	});
}

function create(element_type, class_name = '', child_list = [], callback = obj => { }) {
	let obj = document.createElement(element_type); obj.className = class_name;
	for (let child of child_list) { obj.appendChild(child); }
	callback(obj); return obj;
}

async function load_article_list() {
	let group_list = JSON.parse(await load_file('article_index.json'));
	for (let group of group_list) {
		obj_article_list.appendChild(create('div', '', [
			create('div', 'article-group-text', [], obj => obj.innerText = group.text),
			create('div', '', [], obj => {
				obj.onclick = event => window.location.hash = group.path + '/' + event.target.innerText;
				for (let article of group.list) {
					obj.appendChild(create('div', 'article-link', [], obj => obj.innerText = article))
				}
			})
		]));
		obj_article_list.appendChild(create('br'));
	}
	load_article();
}

async function load_article() {
	article_path = decodeURI(location.hash.substring(1));
	if (article_path == '') {
		obj_article_list.style.display = 'block';
		obj_article.style.display = 'none';
		document.title = 'Chenqi\'s Blog';
	} else {
		obj_article_list.style.display = 'none';
		obj_article.style.display = 'block'; obj_article.scrollIntoView();
		article_title = article_path.substring(article_path.lastIndexOf('/') + 1);
		document.title = article_title + ' - Chenqi\'s Blog';
		obj_article_title.innerText = article_title;
		obj_article_content.innerHTML = '';
		obj_article_content.innerHTML = marked(await load_file(article_path + '.md'));
	}
}

window.onhashchange = load_article;
window.onload = load_article_list;