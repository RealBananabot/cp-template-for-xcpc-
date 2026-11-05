"""Build the A4 two-column booklet from committed templates, without editing code.

Requires reportlab, pypdf and the DejaVu / Droid fallback fonts on this host.
Run from any directory: python3 output/pdf/build_template_pdf.py
"""
from pathlib import Path
import hashlib
import itertools
import json
import re
import subprocess

from reportlab import rl_config
from reportlab.lib.pagesizes import A4
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.pdfgen import canvas
from pypdf import PdfReader

ROOT = Path(__file__).resolve().parents[2]
OUT = Path(__file__).with_name('xcpc-template.pdf')
TMP = ROOT / 'tmp/pdfs/xcpc-template'
TMP.mkdir(parents=True, exist_ok=True)
pdfmetrics.registerFont(TTFont('Code', '/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf'))
pdfmetrics.registerFont(TTFont('CJK', '/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf'))
W, H = A4
MARGIN, GAP, GUTTER = 23, 15, 17
COL = (W - MARGIN * 2 - GAP) / 2
FONT, LEAD = 7.2, 8.3
TOP, BOTTOM = H - 42, 29
SHA = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'], cwd=ROOT, text=True).strip()
TRACKED = set(subprocess.check_output(['git', 'ls-files'], cwd=ROOT, text=True).splitlines())
rl_config.useA85 = False  # Keep compressed streams binary for smaller Git artifacts.

FILES = [
    ('s.cpp', '缺省源'),
    ('static_top_tree.cpp', '静态 Top Tree'),
    ('bipartite_matching.cpp', '二分图最大匹配'),
    ('dinic.cpp', 'Dinic 最大流'),
    ('primal_dual.cpp', 'Primal-Dual 最小费用最大流'),
    ('network_simplex.cpp', '网络单纯形最小费用最大流'),
    ('mint.cpp', '模整数与组合数'),
    ('ntt.cpp', 'NTT 与多项式卷积'),
    ('string/gsam.cpp', '广义后缀自动机'),
    ('string/ac.cpp', 'AC 自动机'),
    ('string/pam.cpp', '回文自动机'),
    ('string/sa.cpp', '后缀数组与相邻后缀 LCP'),
    ('string/lcp_sa.cpp', '后缀数组 LCP / LCS 查询'),
    ('string/kmp.cpp', 'KMP 与 Border'),
    ('string/z.cpp', 'Z 函数与扩展 KMP'),
    ('string/manacher.cpp', 'Manacher 回文半径'),
    ('string/lyndon.cpp', 'Lyndon 分解'),
    ('string/min_rotation.cpp', '最小循环表示'),
    ('string/runs_hash.cpp', 'Runs：单模哈希'),
    ('string/runs_sa.cpp', 'Runs：后缀数组'),
    ('string/runs.cpp', 'Runs：通用查询接口'),
    ('number_theory/exgcd.cpp', '扩展欧几里得与模逆元'),
    ('number_theory/excrt.cpp', '扩展中国剩余定理'),
    ('number_theory/lucas.cpp', 'Lucas 组合数'),
    ('number_theory/exlucas.cpp', '扩展 Lucas 组合数'),
    ('number_theory/miller_rabin_int.cpp', 'Miller-Rabin：int 素性测试'),
    ('number_theory/pollard_rho.cpp', 'Pollard-Rho：整数分解与素性测试'),
    ('number_theory/bsgs.cpp', '扩展 BSGS 离散对数'),
    ('number_theory/primitive_root.cpp', '最小原根：试除'),
    ('number_theory/primitive_root_pollard_rho.cpp', '最小原根：Pollard-Rho'),
    ('number_theory/mod_sqrt.cpp', 'Cipolla 模平方根'),
    ('number_theory/floor_sum.cpp', '类欧几里得：整除和'),
    ('number_theory/floor_sum_mod.cpp', '类欧几里得：整除和取模'),
    ('number_theory/euclid.cpp', '万欧：非交换路径拼接'),
    ('number_theory/floor_moments.cpp', '万欧：高度与平方和'),
    ('number_theory/dujiao.cpp', '杜教筛：欧拉函数前缀和'),
    ('number_theory/min25.cpp', 'Min_25 筛'),
    ('number_theory/pn_sieve.cpp', 'PN 筛'),
    ('number_theory/pn_dujiao.cpp', 'PN 筛与杜教筛'),
    ('number_theory/qdiv.cpp', '倒数乘法除法'),
    ('mint_poly.cpp', '形式幂级数与多项式'),
]

# Match source declarations, rather than relying on brittle fixed line numbers.
POLY = [
    (r'^using u32', '模整数运算'),
    (r'^auto INIT', '单位根、阶乘与逆元预处理'),
    (r'^\s*void DIF', 'NTT：DIF 正变换'),
    (r'^\s*void DIT', 'NTT：DIT 逆变换'),
    (r'^\s*friend poly der\(', '多项式求导'),
    (r'^\s*friend poly ints\(', '多项式积分'),
    (r'^\s*poly& operator \+=', '多项式四则运算与截断'),
    (r'^\s*friend poly conv\(const poly &', '多项式卷积'),
    (r'^\s*friend poly inv\(', '多项式求逆'),
    (r'^\s*friend poly exp\(', '多项式指数'),
    (r'^\s*friend poly ln\(', '多项式对数'),
    (r'^\s*friend poly sqrt\(', '多项式平方根'),
    (r'^\s*friend poly qpow\(', '多项式幂'),
    (r'^\s*friend pair<poly, poly> division', '多项式除法与余数'),
    (r'^\s*friend poly convT\(const poly &', '转置卷积'),
    (r'^\s*friend poly multipoint_eval', '多点求值'),
    (r'^\s*friend polyY conv\(', '二维卷积'),
    (r'^\s*friend polyY convT\(', '二维转置卷积'),
    (r'^\s*friend poly BostanMori\(', 'Bostan-Mori'),
    (r'^\s*friend polyY BostanMoriT\(', '转置 Bostan-Mori'),
    (r'^\s*friend poly composite_inverse', '多项式复合逆'),
    (r'^\s*friend poly composite\(', '多项式复合'),
    (r'^struct dc\{', '在线卷积'),
    (r'^struct dln\s*:', '在线多项式对数'),
    (r'^struct dexp\s*:', '在线多项式指数'),
    (r'^struct dinv\s*:', '在线多项式求逆'),
    (r'^mint C\(', '阶乘组合数'),
    (r'^mint binom\(', '下降幂组合数'),
]

expected = {p for p in TRACKED if p.endswith('.cpp') and '/tests/' not in p}
expected.discard('maxflow_simplex.cpp')
assert {p for p, _ in FILES} == expected, ('Unmapped templates', expected ^ {p for p, _ in FILES})
assert FILES[-1][0] == 'mint_poly.cpp'


def font_for(ch):
    name = 'Code' if ord(ch) in pdfmetrics.getFont('Code').face.charToGlyph else 'CJK'
    assert ord(ch) in pdfmetrics.getFont(name).face.charToGlyph, f'Missing glyph: {ch!r}'
    return name


def width(s, fs=FONT):
    return sum(pdfmetrics.stringWidth(c, font_for(c), fs) for c in s)


def draw_text(c, x, y, s, fs=FONT, color=0):
    c.setFillGray(color)
    for name, chars in itertools.groupby(s, font_for):
        chunk = ''.join(chars)
        c.setFont(name, fs)
        c.drawString(x, y, chunk)
        x += pdfmetrics.stringWidth(chunk, name, fs)


def wrap(line):
    text = line.expandtabs(2).rstrip()
    if not text:
        return ['']
    parts = []
    avail = COL - GUTTER - 3
    while width(text) > avail:
        k, w = 0, 0
        for ch in text:
            cw = width(ch)
            if w + cw > avail:
                break
            k, w = k + 1, w + cw
        # Prefer whitespace; fragments are visual wraps, not new source lines.
        cut = text.rfind(' ', max(1, k // 2), k)
        if cut <= 0:
            cut = k
        else:
            cut += 1
        parts.append(text[:cut])
        text = text[cut:]
    parts.append(text)
    assert ''.join(parts) == line.expandtabs(2).rstrip()
    return parts


def build_layout():
    page, col, y = 2, 0, TOP
    ops, toc, lines = [], [], []

    def space(h):
        nonlocal page, col, y
        if y - h < BOTTOM:
            col += 1
            if col == 2:
                page, col = page + 1, 0
            y = TOP

    def heading(title, key, path=None, sub=False):
        nonlocal y
        need = (27 if sub else 30) + LEAD * 3
        space(need)
        if sub:
            y -= 6
        toc.append(dict(title=title, key=key, page=page, col=col, y=y, sub=sub))
        ops.append(dict(kind='heading', page=page, col=col, y=y, title=title,
                        key=key, path=path, sub=sub))
        y -= 19 if sub else 29

    for idx, (path, title) in enumerate(FILES):
        key = f'file-{idx}'
        heading(title, key, path)
        text = (ROOT / path).read_text()
        committed = subprocess.check_output(['git', 'show', 'HEAD:' + path], cwd=ROOT)
        assert committed == (ROOT / path).read_bytes(), f'Uncommitted source: {path}'
        source = text.splitlines()
        starts = {}
        if path == 'mint_poly.cpp':
            for j, (pattern, name) in enumerate(POLY):
                hits = [i for i, s in enumerate(source, 1) if re.search(pattern, s)]
                assert len(hits) == 1, (name, hits)
                begin = hits[0]
                while begin > 1 and source[begin - 2].lstrip().startswith('//'):
                    begin -= 1
                starts[begin] = (name, f'poly-{j}')
        for num, line in enumerate(source, 1):
            if num in starts:
                name, anchor = starts[num]
                heading(name, anchor, sub=True)
            fragments = wrap(line)
            lines.append(dict(path=path, num=num, fragments=fragments))
            for j, frag in enumerate(fragments):
                space(LEAD)
                ops.append(dict(kind='line', page=page, col=col, y=y, text=frag,
                                num=str(num) if j == 0 else '>', path=path))
                y -= LEAD
        y -= 9
    return ops, toc, lines, page


def draw_toc(c, toc):
    draw_text(c, MARGIN, H - 58, 'XCPC 个人模板', 20)
    draw_text(c, MARGIN, H - 78, '目录', 11)
    draw_text(c, MARGIN, H - 95, 'A4 双栏 / GNU C++20 / 41 份模板 / 多项式代码置于末尾', 8)
    draw_text(c, MARGIN, H - 109, '保留完整源代码；行号旁 > 表示同一行的视觉折行。代码片段按依赖选用。', 7.3)
    start, step = H - 140, 17.2
    rows = 36
    assert len(toc) <= rows * 2
    for i, item in enumerate(toc):
        column, row = divmod(i, rows)
        x, y = MARGIN + column * (COL + GAP), start - row * step
        name = item['title']
        fs = 8.1 if item['sub'] else 8.6
        indent = 9 if item['sub'] else 0
        draw_text(c, x + indent, y, name, fs)
        label = str(item['page'])
        c.setFont('Code', 8)
        c.setFillGray(0)
        c.drawRightString(x + COL - 2, y, label)
        end = x + indent + width(name, fs) + 6
        assert end < x + COL - 20, name
        if end < x + COL - 27:
            c.setStrokeGray(0.75)
            c.setDash(0.7, 2)
            c.line(end, y + 1, x + COL - 22, y + 1)
            c.setDash()
        c.linkRect('', item['key'], (x, y - 3, x + COL, y + 10), relative=0, thickness=0)


def main():
    ops, toc, lines, total = build_layout()
    c = canvas.Canvas(str(OUT), pagesize=A4, pageCompression=1)
    c.setTitle('XCPC 个人模板 - A4 双栏打印版')
    c.setAuthor('RealBananabot')
    c.setSubject(f'41 templates; code snapshot {SHA}; polynomial algorithms indexed separately')
    by_page = {p: [] for p in range(1, total + 1)}
    for op in ops:
        by_page[op['page']].append(op)
    for page in range(1, total + 1):
        draw_text(c, MARGIN, H - 20, 'XCPC / C++20', 7.2, 0.35)
        c.setFont('Code', 6.5)
        c.drawRightString(W - MARGIN, H - 20, 'A4 / TWO COLUMNS')
        c.setStrokeGray(0.7)
        c.setLineWidth(0.35)
        c.line(MARGIN, H - 27, W - MARGIN, H - 27)
        c.line(MARGIN, 21, W - MARGIN, 21)
        draw_text(c, MARGIN, 11, f'Code snapshot {SHA}', 6.5, 0.4)
        c.setFont('Code', 7)
        c.drawRightString(W - MARGIN, 11, f'{page} / {total}')
        if page == 1:
            draw_toc(c, toc)
        else:
            c.setStrokeGray(0.83)
            c.line(W / 2, TOP + 9, W / 2, BOTTOM - 5)
            for op in by_page[page]:
                x, y = MARGIN + op['col'] * (COL + GAP), op['y']
                if op['kind'] == 'heading':
                    c.bookmarkHorizontalAbsolute(op['key'], y + 11, left=x)
                    c.addOutlineEntry(op['title'], op['key'], level=1 if op['sub'] else 0, closed=False)
                    draw_text(c, x, y, op['title'], 9 if op['sub'] else 10)
                    if op['path']:
                        draw_text(c, x, y - 11, op['path'], 6.8, 0.4)
                    c.setStrokeGray(0.7)
                    c.line(x, y - (6 if op['sub'] else 16), x + COL, y - (6 if op['sub'] else 16))
                else:
                    c.setFillGray(0.52)
                    c.setFont('Code', 5.2)
                    c.drawRightString(x + GUTTER - 3, y + 0.2, op['num'])
                    draw_text(c, x + GUTTER, y, op['text'])
                    assert width(op['text']) <= COL - GUTTER - 3 + 0.001
        c.showPage()
    c.save()
    manifest = dict(snapshot=SHA, pages=total, files=[p for p, _ in FILES], toc=toc,
                    source_lines=len(lines), visual_lines=sum(len(l['fragments']) for l in lines),
                    hashes={p: hashlib.sha256((ROOT / p).read_bytes()).hexdigest() for p, _ in FILES})
    (TMP / 'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2))
    (TMP / 'layout.json').write_text(json.dumps(ops, ensure_ascii=False))
    reader = PdfReader(OUT)
    assert len(reader.pages) == total
    assert len(reader.pages[0].get('/Annots', [])) == len(toc)
    for p in reader.pages:
        assert abs(float(p.mediabox.width) - W) < .01
        assert abs(float(p.mediabox.height) - H) < .01
        assert '/Font' in p['/Resources']
    print(json.dumps({k: manifest[k] for k in ['snapshot', 'pages', 'source_lines', 'visual_lines']}, ensure_ascii=False))
    print(OUT)


if __name__ == '__main__':
    main()
