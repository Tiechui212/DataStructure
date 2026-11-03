"""Run against a compiled application: python tests/id_flow_test.py path/to/app.exe."""
import pathlib
import shutil
import subprocess
import sys
import tempfile

exe = pathlib.Path(sys.argv[1]).resolve()
root = pathlib.Path(__file__).resolve().parents[1]


def run(lines, data=None):
    with tempfile.TemporaryDirectory() as folder:
        target = pathlib.Path(folder) / 'papers.txt'
        if data is None:
            shutil.copyfile(root / 'papers.txt', target)
        else:
            target.write_text(data, encoding='utf-8')
        result = subprocess.run(
            [str(exe)], input='\n'.join(lines) + '\n', text=True,
            encoding='utf-8', errors='replace', capture_output=True,
            cwd=folder, timeout=10)
        assert result.returncode == 0, result.stderr
        return result.stdout


def query_results(output):
    return output.split('请输入要查找的论文 ID: ')[1:]


out = run(['6', '1', '6', '1001', '6', '9999', '6', 'abc',
           '6', '1001abc', '6', '2147483648', '6', '', '6', ' 1002 ', '0'])
assert '请先加载论文数据。' in out
results = query_results(out)
assert 'ID: 1001\n标题: Support-Vector Networks' in results[0]
assert all(label in results[0] for label in ['作者:', '关键词:', '年份:', '引用次数:', '摘要:'])
assert '未找到 ID 为 9999' in results[1]
assert all('无效的 ID' in result for result in results[2:6])
assert 'ID: 1002\n标题: The Java Language Specification' in results[6]
print('PASS: unloaded, existing, missing, invalid IDs, detail fields, recovery')

# 1102 and 1001 share a hash bucket (difference 101).
out = run(['1', '3', '1102', 'Integration Paper', 'Test Author',
           'keyword', '', '', '', '', '', '2026', '7', 'Test abstract',
           '6', '1102', '6', '1001', '4', '1001', '6', '1001',
           '6', '1002', '6', '1102', '1', '6', '1102', '6', '1001', '0'])
results = query_results(out)
assert 'ID: 1102\n标题: Integration Paper' in results[0]
assert 'ID: 1001\n标题: Support-Vector Networks' in results[1]
assert '未找到 ID 为 1001' in results[2]
assert 'ID: 1002\n标题: The Java Language Specification' in results[3]
assert 'ID: 1102\n标题: Integration Paper' in results[4]
assert '未找到 ID 为 1102' in results[5]
assert 'ID: 1001\n标题: Support-Vector Networks' in results[6]
print('PASS: add, hash collision, delete, shifted indexes, reload clears old index')

header = (root / 'papers.txt').read_text(encoding='utf-8').splitlines()[0] + '\n'
out = run(['1', '6', '1001', '0'], header)
assert '成功加载 0 条' in out and '未找到 ID 为 1001' in out
print('PASS: empty dataset')

out = run(['1', '6'])
assert '请输入要查找的论文 ID:' in out
print('PASS: EOF exits')
