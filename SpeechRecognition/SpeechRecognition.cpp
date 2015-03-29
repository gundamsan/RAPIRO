#include <stdio.h>

#include <julius/juliuslib.h>

#include "SerialPort.h"


// シリアル通信定義
#define MODEMDEVICE "/dev/ttyAMA0"

// シリアルポートオブジェクト宣言。
SerialPort rapiro(MODEMDEVICE);

// 認識準備完了（喋っていい状態）になった時のコールバック関数定義。
static void callback_speech_ready(Recog *recog, void *dummy)
{
	if (recog->jconf->input.speech_input == SP_MIC
		|| recog->jconf->input.speech_input  == SP_NETAUDIO)
	{
		fprintf(stderr, "<<< please speak >>>\n");
		//rapiro.writeBytes("#M0", 3);
	}
}

// 認識処理開始した時のコールバック関数定義。
static void callback_recognition_begin(Recog *recog, void *dummy)
{
	fprintf(stderr, "--- in recognition ---\n");
	//rapiro.writeBytes("#M8", 3);
}

// 認識結果を出力するコールバック関数定義。
static void callback_output_result(Recog *recog, void *dummy)
{
	WORD_INFO *winfo;
	WORD_ID *seq;
	int seqnum;
	int n, i;
	Sentence *s;
	RecogProcess *r;
	
	for (r = recog->process_list; r; r = r->next)
	{
		// センテンス数だけ繰り返す。
		winfo = r->lm->winfo;
		for (n = 0; n < r->result.sentnum; n++)
		{
			// センテンス構造体のアドレス取得。
			s = &(r->result.sent[n]);
			// ワードの集まりのIDを取得。
			seq = s->word;
			// ワードの数を取得。
			seqnum = s->word_num;

			// ワード数だけ繰り返す。
			for (i = 0; i < seqnum; i++)
			{								
				fprintf(stderr, "%s\n", winfo->woutput[seq[i]]);

				rapiro.writeBytes(winfo->woutput[seq[i]], 3);
			}
		}
	}
	// バッファクリア。
	fflush(stdout);
}


int main(int argc, char *argv[])
{
	Jconf *jconf;	// 設定パラメータ格納用。
	Recog *recog;	// 認識コアのインスタンス。
	int ret;

	// シリアルポートを57600bpsに設定。
	rapiro.begin(57600);

	// -Cオプションで指定する.jconfファイルのチェック。
	if (argc == 1)
	{
		fprintf(stderr, "Try, '-help' for more information.\n");
		rapiro.end();
		return -1;
	}

	// 指定した.jconfファイルから設定を読み込む。
	jconf = j_config_load_args_new(argc, argv);
	if (jconf == NULL)
	{
		fprintf(stderr, "Error in j_config_load_args_new\n");
		rapiro.end();
		return -1;
	}

	// 読み込んだjconfから認識器を作成。
	recog = j_create_instance_from_jconf(jconf);
	if (recog == NULL)
	{
		fprintf(stderr, "Error in j_create_instance_from_jconf\n");
		rapiro.end();
		return -1;
	}

	// コールバックの設定。
	callback_add(recog, CALLBACK_EVENT_SPEECH_READY, callback_speech_ready, NULL);	// 認識準備完了。
	callback_add(recog, CALLBACK_EVENT_RECOGNITION_BEGIN, callback_recognition_begin, NULL);	// 認識処理開始。
	callback_add(recog, CALLBACK_RESULT, callback_output_result, NULL);	// 認識結果出力。

	// マイクなどオーディオ周りの初期化。
	if (j_adin_init(recog) == FALSE)
	{
		// Error.
		fprintf(stderr, "Error in j_adin_init\n");
		rapiro.end();
		return -1;
	}

	// ログ出力有効。
	j_recog_info(recog);

	// 入力デバイスがオープンされているか確認。
	switch (j_open_stream(recog, NULL))
	{
	case 0:		// 正常。
		break;
	case -1:	// エラー。
		fprintf(stderr, "error in input stream\n");
		return 0;
	case -2:
		fprintf(stderr, "failed to begininput stream\n");
		return 0;
	}

	// 認識を行うループに入る。
	ret = j_recognize_stream(recog);
	// 処理内部でエラーが起こったかどうか。
	if (ret == -1)	return -1;

	// 終了処理。
	// ストリームを閉じる。
	j_close_stream(recog);
	// インスタンスを解放。
	j_recog_free(recog);

	// シリアルポート終了。
	rapiro.end();

	return 0;
}